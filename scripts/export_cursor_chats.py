# new file: export_cursor_chats.py
import os
from datetime import datetime
import sqlite3
import json
from collections import defaultdict

def export_chats():
    # Locate the Cursor state database
    db_path = os.path.expanduser('~/Library/Application Support/Cursor/User/globalStorage/state.vscdb')
    if not os.path.exists(db_path):
        print(f'Database not found: {db_path}')
        return

    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()
    cursor.execute("SELECT key, value FROM cursorDiskKV WHERE key LIKE 'bubbleId:%'")

    chats = defaultdict(list)
    for key, blob in cursor.fetchall():
        parts = key.split(':')
        if len(parts) < 3:
            continue
        chat_id = parts[1]
        msg_id = parts[2]
        try:
            data = json.loads(blob)
        except Exception:
            continue
        chats[chat_id].append((msg_id, data))

    # Base directory for exported chats
    output_dir = os.path.join(os.getcwd(), 'docs', 'chat_history')
    os.makedirs(output_dir, exist_ok=True)

    for chat_id, msgs in chats.items():
        # Determine chat title from composerData metadata
        title = chat_id
        meta_key = f'composerData:{chat_id}'
        cursor.execute("SELECT value FROM cursorDiskKV WHERE key = ?", (meta_key,))
        row = cursor.fetchone()
        if row and row[0]:
            try:
                meta = json.loads(row[0])
                if meta.get('name'):
                    title = meta['name']
            except Exception:
                pass
        # Sanitize title for filename
        safe_title = ''.join(c if c.isalnum() or c in (' ', '-', '_') else '_' for c in title).strip()[:100] or chat_id
        # Determine output subdirectory and date prefix if available
        subdir = None
        date_prefix = ''
        if row and row[0]:
            try:
                created = meta.get('createdAt') or meta.get('created')
                if created:
                    # parse ISO8601 timestamp
                    dt = datetime.fromisoformat(created)
                    subdir = dt.strftime('%Y-%m')
                    date_prefix = dt.strftime('%Y-%m-%d_')
            except Exception:
                pass
        # prepare folder
        folder = os.path.join(output_dir, subdir) if subdir else output_dir
        os.makedirs(folder, exist_ok=True)
        # Sort messages by message ID (lexicographically)
        msgs.sort(key=lambda x: x[0])
        lines = []
        for msg_id, data in msgs:
            role = 'assistant' if data.get('type') == 2 else 'user'
            text = data.get('text', '').strip()
            lines.append(f'### {role} ({msg_id})')
            lines.append(text)
            lines.append('')

        # prefix date to filename
        filename = f'{date_prefix}{safe_title}.md'
        filepath = os.path.join(folder, filename)
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write('\n'.join(lines))

    conn.close()
    print(f'Exported {len(chats)} chats to {output_dir}')

if __name__ == '__main__':
    export_chats() 