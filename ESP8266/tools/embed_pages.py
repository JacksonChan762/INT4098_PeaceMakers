"""Embed the editable offline teacher HTML page in ESP8266 flash (PROGMEM)."""
from pathlib import Path
root = Path(__file__).resolve().parents[1]
parts = ['#pragma once', '#include <Arduino.h>', '']
for name, filename in [('TEACHER_PAGE', 'teacher.html')]:
    html = (root / 'web' / filename).read_text(encoding='utf-8')
    marker = 'STBHTML'
    assert f'){marker}"' not in html, 'Raw string literal delimiter collision'
    parts.append(f'const char {name}[] PROGMEM = R"{marker}({html}){marker}";')
    parts.append('')
(root / 'src/pages.h').write_text('\n'.join(parts), encoding='utf-8')
print('Generated ESP8266/src/pages.h')
