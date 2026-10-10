--==============================================================================
-- [DOG] 注入动效 · 传承版（YimMenu Lua 版）
--------------------------------------------------------------------------------
-- 每次注入菜单 DLL（脚本加载）后自动播放一次，约 8 秒（与增强版动效同款节奏）：
--   0.00-0.90s  光带从中心向两侧展开（HUD 启动感）
--   0.60-1.80s  主字「注入成功」从 0.8 倍弹性放大入场（青绿 + 辉光）
--   1.20-2.20s  署名「D O G」宽字距浮现（金色，两侧细线同步展开）
--   1.90-2.80s  高光从左向右扫过整组文字
--   2.80-6.60s  呼吸停留（辉光强度 ±15% 波动）
--   6.60-8.00s  谢幕：整体上浮渐隐、光带收拢
--
-- 实现：gui.add_always_draw_imgui（菜单关闭也逐帧绘制）+ ImGui 前景 DrawList。
-- 屏幕尺寸经 GRAPHICS.GET_SCREEN_RESOLUTION 在 fiber 中获取一次（带重试）。
-- 全部节奏 / 配色 / 尺寸见下方 CFG，与增强版 Onboarding.cpp 常量一一对应。
--==============================================================================

local CFG = {
    -- 时间轴（秒）
    duration        = 8.00,
    band_open_end   = 0.90,
    title_in_start  = 0.60,
    title_in_end    = 1.80,
    dog_in_start    = 1.20,
    dog_in_end      = 2.20,
    shimmer_start   = 1.90,
    shimmer_end     = 2.80,
    hold_end        = 6.60,
    fade_end        = 8.00,

    -- 配色（RGB 0-1）
    title_color = { 0.36, 0.95, 0.76 }, -- 主字：青绿
    dog_color   = { 1.00, 0.84, 0.36 }, -- 署名：金色
    band_color  = { 0.30, 0.90, 0.76 }, -- 光带：青绿

    -- 布局
    title_size       = 54.0,  -- 主字号
    dog_size         = 22.0,  -- 署名字号
    band_width       = 560.0, -- 光带最大宽度
    band_thickness   = 3.0,   -- 光带厚度
    band_glow_height = 40.0,  -- 光带下方光晕高度
    dog_gap          = 44.0,  -- 署名字距
    dog_line_len     = 96.0,  -- 署名两侧细线长度
    rise_distance    = 18.0,  -- 谢幕上浮距离
    anchor_y         = 0.42,  -- 纵向锚点（屏高 42%，中央偏上）
}

--== 基础数学 ==================================================================

local function clamp(v, lo, hi)
    if v < lo then return lo end
    if v > hi then return hi end
    return v
end

local function lerp(a, b, t) return a + (b - a) * t end

local function ease_out_cubic(t)
    t = clamp(t, 0.0, 1.0)
    local u = 1.0 - t
    return 1.0 - u * u * u
end

local function ease_out_back(t)
    t = clamp(t, 0.0, 1.0)
    local c1 = 1.70158
    local c3 = c1 + 1.0
    local u = t - 1.0
    return 1.0 + c3 * u * u * u + c1 * u * u
end

local function ease_in_out_cubic(t)
    t = clamp(t, 0.0, 1.0)
    if t < 0.5 then
        return 4.0 * t * t * t
    end
    local u = -2.0 * t + 2.0
    return 1.0 - (u * u * u) / 2.0
end

--== ImGui 辅助 ================================================================

local function col(c, a)
    return ImGui.ColorConvertFloat4ToU32({ c[1], c[2], c[3], clamp(a, 0.0, 1.0) })
end

local function white(a)
    return ImGui.ColorConvertFloat4ToU32({ 1.0, 1.0, 1.0, clamp(a, 0.0, 1.0) })
end

local base_font_size = nil

--- 计算指定字号下文本尺寸（按默认字体线性缩放）
local function text_size(size, text)
    if base_font_size == nil then
        base_font_size = ImGui.GetFontSize()
        if base_font_size == nil or base_font_size <= 0 then base_font_size = 20.0 end
    end
    local w, h = ImGui.CalcTextSize(text)
    local s = size / base_font_size
    return w * s, h * s
end

local kDirs = {
    { 1.0, 0.0 }, { -1.0, 0.0 }, { 0.0, 1.0 }, { 0.0, -1.0 },
    { 0.7, 0.7 }, { -0.7, 0.7 }, { 0.7, -0.7 }, { -0.7, -0.7 },
}

--- 带彩色辉光与黑色软描边的文字（任意背景下可读）
local function add_glow_text(dl, size, x, y, color, text, alpha, glow)
    if alpha <= 0.001 then return end

    -- 彩色外辉光（两圈）
    if glow and glow > 0.01 then
        for ring = 0, 1 do
            local dist = 3.0 + ring * 3.0
            local ring_alpha = (ring == 0 and 0.22 or 0.12) * glow * alpha
            local c = col(color, ring_alpha)
            for i = 1, 8 do
                ImGui.ImDrawListAddText(dl, size, x + kDirs[i][1] * dist, y + kDirs[i][2] * dist, c, text)
            end
        end
    end

    -- 黑色软描边
    local outline = ImGui.ColorConvertFloat4ToU32({ 0.0, 0.0, 0.0, clamp(140.0 / 255.0 * alpha, 0.0, 1.0) })
    for i = 1, 8 do
        ImGui.ImDrawListAddText(dl, size, x + kDirs[i][1] * 2.2, y + kDirs[i][2] * 2.2, outline, text)
    end

    -- 本体
    ImGui.ImDrawListAddText(dl, size, x, y, col(color, alpha), text)
end

--== 运行状态 ==================================================================

local screen_w, screen_h = nil, nil
local last_probe = -100.0
local t0 = nil
local done = false

--- 在 fiber 中获取游戏分辨率（需要脚本线程上下文，失败则下次重试）
local function probe_resolution()
    script.run_in_fiber(function()
        local ok, x, y = pcall(function()
            return GRAPHICS.GET_SCREEN_RESOLUTION(0, 0)
        end)
        if ok and type(x) == "number" and type(y) == "number" and x > 0 and y > 0 then
            screen_w, screen_h = x, y
        end
    end)
end

--== 每帧绘制 ==================================================================

local function draw_fx()
    if done then return end

    local now = ImGui.GetTime()

    -- 分辨率未知：每秒重试一次，拿到后再开始计时
    if screen_w == nil or screen_h == nil then
        if now - last_probe > 1.0 then
            last_probe = now
            probe_resolution()
        end
        return
    end

    if t0 == nil then t0 = now end
    local t = now - t0
    if t >= CFG.duration then
        done = true
        return
    end

    local cx = screen_w * 0.5
    local cy = screen_h * CFG.anchor_y

    -- 全局时间轴派生量
    local alpha = 1.0
    local rise = 0.0
    if t > CFG.hold_end then
        local p = ease_in_out_cubic((t - CFG.hold_end) / (CFG.fade_end - CFG.hold_end))
        alpha = 1.0 - p
        rise = p * CFG.rise_distance
    end

    local breath = 1.0
    if t > 2.8 and t < CFG.hold_end then
        breath = 1.0 + 0.15 * math.sin((t - 2.8) * 2.0 * math.pi / 1.2)
    end

    local origin_y = cy - rise
    local dl = ImGui.GetForegroundDrawList()

    local band_y = origin_y - 92.0
    local title_y = origin_y - 44.0
    local dog_y = origin_y + 40.0

    -- ① 光带 -----------------------------------------------------------------
    do
        local band_p = ease_out_cubic(t / CFG.band_open_end)
        if t > CFG.hold_end then
            band_p = band_p * (1.0 - ease_in_out_cubic((t - CFG.hold_end) / (CFG.fade_end - CFG.hold_end)))
        end

        local half_w = CFG.band_width * 0.5 * band_p
        if half_w > 1.0 then
            local a = alpha * 0.95
            local edge = col(CFG.band_color, 0.0)
            local core = col(CFG.band_color, a)

            ImGui.ImDrawListAddRectFilledMultiColor(dl, cx - half_w, band_y, cx, band_y + CFG.band_thickness, edge, core, core, edge)
            ImGui.ImDrawListAddRectFilledMultiColor(dl, cx, band_y, cx + half_w, band_y + CFG.band_thickness, core, edge, edge, core)

            local halo = col(CFG.band_color, a * 0.16)
            ImGui.ImDrawListAddRectFilledMultiColor(dl, cx - half_w, band_y + CFG.band_thickness, cx + half_w, band_y + CFG.band_thickness + CFG.band_glow_height, halo, halo, edge, edge)
        end
    end

    -- ② 主字「注入成功」 ------------------------------------------------------
    local title_min_x, title_max_x, title_top, title_bottom
    do
        if t >= CFG.title_in_start then
            local p = ease_out_back((t - CFG.title_in_start) / (CFG.title_in_end - CFG.title_in_start))
            local fade_in = clamp((t - CFG.title_in_start) / 0.35, 0.0, 1.0)
            local size = CFG.title_size * (0.8 + 0.2 * p)
            local text = "注入成功"
            local w, h = text_size(size, text)
            local x = cx - w * 0.5
            local y = title_y - h * 0.5

            title_min_x = x
            title_max_x = x + w
            title_top = y
            title_bottom = y + h

            add_glow_text(dl, size, x, y, CFG.title_color, text, fade_in * alpha, breath)
        end
    end

    -- ③ 署名「D O G」 ---------------------------------------------------------
    local dog_min_x, dog_max_x, dog_bottom
    do
        if t >= CFG.dog_in_start then
            local p = ease_out_cubic((t - CFG.dog_in_start) / (CFG.dog_in_end - CFG.dog_in_start))
            local letters = { "D", "O", "G" }
            local widths = {}
            local total = 0.0
            for i = 1, 3 do
                widths[i] = text_size(CFG.dog_size, letters[i])
                total = total + widths[i] + (i < 3 and CFG.dog_gap or 0.0)
            end

            local _, dog_h = text_size(CFG.dog_size, "D")
            local x = cx - total * 0.5
            dog_min_x = x
            dog_max_x = x + total
            dog_bottom = dog_y + dog_h

            for i = 1, 3 do
                add_glow_text(dl, CFG.dog_size, x, dog_y, CFG.dog_color, letters[i], p * alpha, 0.45)
                x = x + widths[i] + CFG.dog_gap
            end

            -- 两侧细线（随出场进度从内向外展开）
            local line_len = CFG.dog_line_len * p
            local line_y = dog_y + dog_h * 0.5
            local line_gap = 18.0
            if line_len > 1.0 then
                local t_edge = col(CFG.dog_color, 0.0)
                local t_core = col(CFG.dog_color, 0.7 * alpha)
                ImGui.ImDrawListAddRectFilledMultiColor(dl, dog_min_x - line_gap - line_len, line_y - 1.0, dog_min_x - line_gap, line_y + 1.0, t_edge, t_core, t_core, t_edge)
                ImGui.ImDrawListAddRectFilledMultiColor(dl, dog_max_x + line_gap, line_y - 1.0, dog_max_x + line_gap + line_len, line_y + 1.0, t_core, t_edge, t_edge, t_core)
            end
        end
    end

    -- ④ 扫光（掠过整组文字；等价于原版裁剪扫描 + 边界透明度渐变） --------------
    if t >= CFG.shimmer_start and t <= CFG.shimmer_end and title_min_x and title_max_x and title_max_x > title_min_x then
        local p = ease_out_cubic((t - CFG.shimmer_start) / (CFG.shimmer_end - CFG.shimmer_start))
        local left = title_min_x - 20.0
        local right = title_max_x + 20.0
        local top = title_top - 12.0
        local bottom = math.max(dog_bottom or 0.0, title_bottom or 0.0) + 12.0
        local scan_x = lerp(left - 60.0, right + 60.0, p)
        local scan_w = 64.0
        local bright = 140.0 / 255.0 * alpha

        -- 段 A：[scan_x - scan_w, scan_x]，透明 → 亮
        local xa1 = math.max(scan_x - scan_w, left)
        local xa2 = math.min(scan_x, right)
        if xa2 > xa1 then
            local a1 = bright * clamp((xa1 - (scan_x - scan_w)) / scan_w, 0.0, 1.0)
            local a2 = bright * clamp((xa2 - (scan_x - scan_w)) / scan_w, 0.0, 1.0)
            ImGui.ImDrawListAddRectFilledMultiColor(dl, xa1, top, xa2, bottom, white(a1), white(a2), white(a2), white(a1))
        end

        -- 段 B：[scan_x, scan_x + scan_w]，亮 → 透明
        local xb1 = math.max(scan_x, left)
        local xb2 = math.min(scan_x + scan_w, right)
        if xb2 > xb1 then
            local a1 = bright * (1.0 - clamp((xb1 - scan_x) / scan_w, 0.0, 1.0))
            local a2 = bright * (1.0 - clamp((xb2 - scan_x) / scan_w, 0.0, 1.0))
            ImGui.ImDrawListAddRectFilledMultiColor(dl, xb1, top, xb2, bottom, white(a1), white(a2), white(a2), white(a1))
        end
    end
end

--== 注册（菜单关闭也逐帧绘制；每次注入自动重播） ==============================

local ok, err = pcall(function()
    gui.add_always_draw_imgui(draw_fx)
end)

if not ok then
    pcall(function()
        print("[DOG_InjectFx] 注册失败（当前菜单版本不支持 gui.add_always_draw_imgui）: " .. tostring(err))
    end)
end
