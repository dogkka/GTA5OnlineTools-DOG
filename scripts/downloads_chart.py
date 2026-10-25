#!/usr/bin/env python3
"""每日抓取 Release 下载量并生成折线图。

GitHub 只提供“当前累计下载量”,没有历史曲线;所以这里每天追加一条快照,
再用纯手写 SVG 画出来(不依赖 matplotlib,Action 里不用装任何包)。
"""
import datetime
import json
import os
import urllib.request

REPO = "dogkka/GTA5OnlineTools-DOG"
DATA = "data/downloads.json"
OUT = "assets/downloads.svg"


def fetch_releases():
    req = urllib.request.Request(
        f"https://api.github.com/repos/{REPO}/releases?per_page=100",
        headers={"User-Agent": "k9-downloads-chart", "Accept": "application/vnd.github+json"},
    )
    token = os.environ.get("GH_TOKEN") or os.environ.get("GITHUB_TOKEN")
    if token:
        req.add_header("Authorization", f"Bearer {token}")
    with urllib.request.urlopen(req, timeout=30) as resp:
        return json.load(resp)


def snapshot():
    releases = fetch_releases()
    per = {}
    total = 0
    for rel in releases:
        if rel.get("draft"):
            continue
        # 本仓库只发布小助手;K9 是独立产品线,不把它的下载量算进来
        if rel["tag_name"].lower().startswith("k9"):
            continue
        n = sum(int(a.get("download_count", 0)) for a in rel.get("assets", []))
        per[rel["tag_name"]] = n
        total += n
    return total, per


def render(hist):
    W, H = 860, 280
    pad_l, pad_r, pad_t, pad_b = 64, 24, 28, 42
    ys = [int(h["total"]) for h in hist]
    n = len(ys)
    max_y = max(ys) if ys else 1
    # 取一个好看的刻度上限
    step = max(1, 10 ** max(0, len(str(max_y)) - 1))
    top = ((max_y + step - 1) // step) * step or 1

    def px(i):
        if n <= 1:
            return (pad_l + W - pad_r) / 2
        return pad_l + (W - pad_l - pad_r) * i / (n - 1)

    def py(v):
        return pad_t + (H - pad_t - pad_b) * (1 - v / top)

    parts = [
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}" role="img" aria-label="Downloads over time">',
        "<defs>",
        '<linearGradient id="g" x1="0" y1="0" x2="0" y2="1">',
        '<stop offset="0%" stop-color="#2f81f7" stop-opacity="0.35"/>',
        '<stop offset="100%" stop-color="#2f81f7" stop-opacity="0"/>',
        "</linearGradient>",
        "</defs>",
        '<rect width="100%" height="100%" fill="#ffffff"/>',
    ]

    # 横向网格 + Y 轴刻度
    grid_lines = 4
    for k in range(grid_lines + 1):
        v = top * k / grid_lines
        y = py(v)
        parts.append(
            f'<line x1="{pad_l}" y1="{y:.1f}" x2="{W - pad_r}" y2="{y:.1f}" stroke="#e6e9ef" stroke-width="1"/>'
        )
        parts.append(
            f'<text x="{pad_l - 8}" y="{y + 4:.1f}" font-family="ui-monospace,monospace" font-size="11" fill="#8b95a7" text-anchor="end">{int(v)}</text>'
        )

    if n:
        pts = [(px(i), py(v)) for i, v in enumerate(ys)]
        line = " ".join(f"{x:.1f},{y:.1f}" for x, y in pts)
        area = f"{pts[0][0]:.1f},{py(0):.1f} " + line + f" {pts[-1][0]:.1f},{py(0):.1f}"
        parts.append(f'<polygon points="{area}" fill="url(#g)"/>')
        parts.append(
            f'<polyline points="{line}" fill="none" stroke="#2f81f7" stroke-width="2.4" stroke-linejoin="round" stroke-linecap="round"/>'
        )
        for x, y in pts:
            parts.append(f'<circle cx="{x:.1f}" cy="{y:.1f}" r="3" fill="#2f81f7"/>')

        # 首尾日期
        parts.append(
            f'<text x="{pad_l}" y="{H - 14}" font-family="ui-monospace,monospace" font-size="11" fill="#8b95a7">{hist[0]["date"]}</text>'
        )
        if n > 1:
            parts.append(
                f'<text x="{W - pad_r}" y="{H - 14}" font-family="ui-monospace,monospace" font-size="11" fill="#8b95a7" text-anchor="end">{hist[-1]["date"]}</text>'
            )

        # 最新值
        parts.append(
            f'<text x="{W - pad_r}" y="{pad_t - 8}" font-family="ui-monospace,monospace" font-size="12" fill="#1f2328" text-anchor="end">total {ys[-1]}</text>'
        )

    parts.append("</svg>")
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w", encoding="utf-8") as f:
        f.write("\n".join(parts))


def main():
    total, per = snapshot()
    os.makedirs("data", exist_ok=True)
    hist = []
    if os.path.exists(DATA):
        with open(DATA, encoding="utf-8") as f:
            hist = json.load(f)
    today = datetime.date.today().isoformat()
    if hist and hist[-1].get("date") == today:
        hist[-1]["total"] = total
        hist[-1]["per_release"] = per
    else:
        hist.append({"date": today, "total": total, "per_release": per})
    with open(DATA, "w", encoding="utf-8") as f:
        json.dump(hist, f, ensure_ascii=False, indent=2)
    render(hist)
    print(f"[chart] {today} total={total} releases={len(per)} points={len(hist)}")


if __name__ == "__main__":
    main()
