import sys
import re
import json
import csv
import string
import struct

# Validate arguments
if len(sys.argv) != 4:
    print("Usage: python3 evaluation.py <student_id> <question_id> <testcase_id>")
    sys.exit(1)

student_id = sys.argv[1]
question_id = sys.argv[2]
testcase_id = sys.argv[3]

PRINTABLE_HEX = {format(ord(c), '02x') for c in string.printable if c not in '\r\n\t\x0b\x0c'}

# ----------- Parse Port Mappings -----------

def parse_ports():
    ip_to_logical = {}

    # Client ports
    with open("clientPorts.sh") as f:
        for line in f:
            line = line.strip()
            match = re.match(r'CLIENT_PORT\[(\d+)\]=(\d+)', line)
            if match:
                index, port = match.groups()
                ip = f"127.0.0.1.{port}"
                ip_to_logical[ip] = f"client{int(index)+1}"

    # Server ports
    with open("serverPorts.sh") as f:
        for line in f:
            line = line.strip()
            match = re.match(r'SERVER_PORT\[(\d+)\]=(\d+)', line)
            if match:
                index, port = match.groups()
                ip = f"127.0.0.1.{port}"
                ip_to_logical[ip] = f"server{int(index)+1}"

    return ip_to_logical

ip_to_logical = parse_ports()

def get_logical(ip):
    return ip_to_logical[ip]

def normalize_ip_port(ip_str):
    if ':' in ip_str:
        ip, port = ip_str.split(':')
        return f"{ip}.{port}"
    return ip_str

# ----------- Load Testcases and Extract Target One -----------
with open("testcases.json") as f:
    testcases = json.load(f)

if question_id not in testcases or testcase_id not in testcases[question_id]:
    print(f"Testcase {question_id}.{testcase_id} not found in testcases.json")
    sys.exit(1)

pairs = testcases[question_id][testcase_id]

# ----------- Parse hex_transfer.log -----------
entries = []
with open("hex_transfer.log") as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        parts = line.split(",")
        if len(parts) != 4:
            continue
        raw_src, raw_dst, length, hex_payload = parts
        try:
            src = normalize_ip_port(raw_src.strip())
            dst = normalize_ip_port(raw_dst.strip().rstrip(":"))
        except ValueError:
            continue
        try:
            src_id = get_logical(src)
            dst_id = get_logical(dst)
        except KeyError:
            continue
        length = int(length.strip())
        payload = hex_payload.strip().replace(" ", "").lower()

        if "client" in src_id:
            direction = "c->s"
        elif "server" in src_id:
            direction = "s->c"
        else:
            continue

        entries.append((src_id, dst_id, direction, payload))

# ----------- Convert testcase data to HEX -----------
def to_hex_variants(data):
    if isinstance(data, str):
        hex_str = data.encode('utf-8').hex()
        return (hex_str, hex_str)
    elif isinstance(data, int):
        return (
            struct.pack('>i', data).hex(),
            struct.pack('<i', data).hex()
        )
    elif isinstance(data, float):
        return (
            struct.pack('>d', data).hex(),
            struct.pack('<d', data).hex()
        )
    elif isinstance(data, bool):
        return (
            struct.pack('>i', 1 if data else 0).hex(),
            struct.pack('<i', 1 if data else 0).hex()
        )
    else:
        raise TypeError(f"Unsupported data type: {type(data)}")

# ----------- Evaluation Logic -----------
row = [student_id, f"{question_id}.{testcase_id}"]

# Initialize dynamic columns (2 per communication)
for _ in pairs:
    row.append("fail")
    row.append("wrong")

for idx, comm in enumerate(pairs):  # 'pairs' is now a list of dicts
    if len(comm) != 1:
        print(f"Invalid communication entry in testcase: {comm}")
        continue

    key, expected_data = next(iter(comm.items()))
    src_logical, dst_logical = key.split("_to_")
    direction = "c->s" if "client" in src_logical else "s->c"

    for e_src, e_dst, e_dir, e_data in entries:
        if e_dir == direction and e_src == src_logical and e_dst == dst_logical:
            try:
                hex_big, hex_little = to_hex_variants(expected_data)
            except Exception as e:
                print(f"Hex conversion failed for data {expected_data}: {e}")
                continue

            actual = e_data.lower()
            matched = False

            if isinstance(expected_data, (int, float, bool)):
                matched = (actual == hex_big) or (actual == hex_little)
            elif isinstance(expected_data, str):
                if actual.startswith(hex_big):
                    next_index = len(hex_big)
                    if next_index + 2 <= len(actual):
                        next_byte = actual[next_index:next_index+2]
                        if next_byte in PRINTABLE_HEX:
                            matched = False
                        else:
                            matched = True
                    else:
                        matched = True
                elif actual.startswith(hex_little):
                    next_index = len(hex_little)
                    if next_index + 2 <= len(actual):
                        next_byte = actual[next_index:next_index+2]
                        if next_byte in PRINTABLE_HEX:
                            matched = False
                        else:
                            matched = True
                    else:
                        matched = True

            row[2 + 2*idx] = "ok"
            if matched:
                row[2 + 2*idx + 1] = "correct"
                break

# ----------- Append to evaluated.csv -----------
eval_file = f"{student_id}_evaluated.csv"

with open(eval_file, "a", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(row)
    
print(f"Appended result for {question_id}.{testcase_id} to {eval_file}")
