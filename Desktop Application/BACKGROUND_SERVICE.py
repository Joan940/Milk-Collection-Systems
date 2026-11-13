import os
import sys
import time
import socket
import logging
import hashlib
import psycopg2
import threading
import base64

from datetime import datetime
from Crypto.Cipher import AES
from Crypto.Util.Padding import unpad
from supabase import create_client, Client

# KONFIGURASI KEAMANAN & LOKAL
KUNCI_AES = b"12345678901234561234567890123456"
IV_AES    = b"1234567890123456"

SECRET_KEY_HASH = "KopengKUD_SecureKey_2026!#"

DB_LOCAL = {
    "host": "127.0.0.1",
    "database": "Data_Peternak",
    "user": "postgres",
    "password": "kipaza08",
    "port": "5432"
}

# KONFIGURASI CLOUD (SUPABASE)
SUPABASE_URL = "https://qrummydcfiftbkmgebkq.supabase.co"
SUPABASE_KEY = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InFydW1teWRjZmlmdGJrbWdlYmtxIiwicm9sZSI6InNlcnZpY2Vfcm9sZSIsImlhdCI6MTc3ODAyMTIxMywiZXhwIjoyMDkzNTk3MjEzfQ.IY_xUfIRVZtr9Li_AzUHYoTzZb6NxXB8U9dYUsV4jOs"
supabase: Client = create_client(SUPABASE_URL, SUPABASE_KEY)

# KONFIGURASI JARINGAN TCP
SERVER_IP = "0.0.0.0"
SERVER_PORT = 5005

if getattr(sys, 'frozen', False):
    folder_utama = os.path.dirname(sys.executable)
else:
    folder_utama = os.path.dirname(__file__)

log_file = os.path.join(folder_utama, "service_tcp.log")
logging.basicConfig(filename=log_file, level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')


def buat_token_sha256(id_peternak, id_kios, volume, waktu_str):
    id_ptn_str  = str(id_peternak).strip()
    id_kios_str = str(id_kios).strip()    
    vol_str = f"{float(volume):.2f}"
    payload_mentah = f"{id_ptn_str}{id_kios_str}{vol_str}{waktu_str}{SECRET_KEY_HASH}"

    return hashlib.sha256(payload_mentah.encode('utf-8')).hexdigest()

def simpan_dan_sinkron(uid_rfid, id_kios, volume, waktu_node):
    conn = None
    try:
        try:
            objek_waktu = datetime.strptime(waktu_node, "%d/%m/%Y %H:%M")
            waktu_standar = objek_waktu.strftime("%Y-%m-%d %H:%M:%S")
        except Exception as e:
            logging.warning(f"Gagal parse waktu node, menggunakan waktu asli: {e}")
            waktu_standar = waktu_node

        conn = psycopg2.connect(**DB_LOCAL)
        cur = conn.cursor()
        
        cur.execute("SELECT id_peternak, id_kios_terdaftar FROM tb_peternak WHERE uid_rfid = %s", (uid_rfid,))
        result = cur.fetchone()

        if result:
            id_peternak = result[0]
            id_kios_terdaftar = str(result[1]).strip() if result[1] else None
            id_kios_dari_node = str(id_kios).strip()

            # if id_kios_terdaftar != id_kios_dari_node:
            #     pesan_tolak = f"[DITOLAK] Peternak #{id_peternak} terdaftar di Kios '{id_kios_terdaftar}', tapi mencoba setor di Kios '{id_kios_dari_node}'!"
            #     print(f"\n❌ {pesan_tolak}")
            #     logging.warning(pesan_tolak)
                
            #     cur.close()
            #     conn.close()
            #     return

            waktu_mulai = time.perf_counter()
            token_hash = buat_token_sha256(id_peternak, id_kios, volume, waktu_standar)
            waktu_selesai = time.perf_counter()
            waktu_komputasi_ms = (waktu_selesai - waktu_mulai) * 1000
            print(f"Hasil Enkripsi SHA-256  : {token_hash}")
            print(f"Waktu Komputasi SHA-256 : {waktu_komputasi_ms:.4f} ms")

            harga_saat_ini = 7500
            try:
                cur.execute("SELECT harga_harian FROM pengaturan_sistem ORDER BY id DESC LIMIT 1")
                result_harga = cur.fetchone()
                
                if result_harga and result_harga[0] is not None:
                    harga_saat_ini = float(result_harga[0])
                    
            except Exception as e:
                logging.warning(f"[LOKAL] Gagal tarik harga harian dari DB Lokal, pakai default 7500: {e}")

            total_pendapatan = float(volume) * harga_saat_ini
            
            query_insert = """
                INSERT INTO tb_transaksi (id_peternak, id_kios_penyetoran, volume_liter, waktu_setor, token, harga_per_liter, total_pendapatan)
                VALUES (%s, %s, %s, %s, %s, %s, %s) RETURNING id_transaksi;
            """
            cur.execute(query_insert, (id_peternak, id_kios, volume, waktu_node, token_hash, harga_saat_ini, total_pendapatan))
            id_transaksi_lokal = cur.fetchone()[0]
            conn.commit()
            logging.info(f"[LOKAL] Transaksi #{id_transaksi_lokal} tersimpan. Token: {token_hash[:10]}...")

            try:
                data_cloud = {
                    "id_transaksi": id_transaksi_lokal, 
                    "id_peternak": id_peternak,
                    "id_kios_penyetoran": id_kios,
                    "volume_liter": volume,
                    "waktu_setor": waktu_standar,  
                    "token": token_hash,
                    "harga_per_liter": harga_saat_ini,
                    "total_pendapatan": total_pendapatan
                }
                supabase.table("tb_transaksi").upsert(data_cloud).execute()
                cur.execute("UPDATE tb_transaksi SET status_sinkron = TRUE WHERE id_transaksi = %s", (id_transaksi_lokal,))
                conn.commit()
                logging.info(f"[CLOUD] Transaksi #{id_transaksi_lokal} berhasil disinkronisasi ke Supabase.")
            except Exception as e:
                logging.error(f"[CLOUD] Gagal sinkron ke Supabase (Internet mati?): {e}")

        else:
            logging.warning(f"UID RFID '{uid_rfid}' tidak ditemukan di tabel lokal tb_peternak.")

        cur.close()
    except Exception as e:
        logging.error(f"Gagal operasi Database: {e}")
    finally:
        if conn:
            conn.close()

def jalankan_sinkronisasi_susulan():
    while True:
        time.sleep(60)
        conn = None
        try:
            conn = psycopg2.connect(**DB_LOCAL)
            cur = conn.cursor()
            
            cur.execute("SELECT id_transaksi, id_peternak, id_kios_penyetoran, volume_liter, waktu_setor, token, harga_per_liter, total_pendapatan FROM tb_transaksi WHERE status_sinkron = FALSE;")
            data_tertunda = cur.fetchall()
            
            if data_tertunda:
                logging.info(f"[SYNC] Menemukan {len(data_tertunda)} data tertunda. Mencoba mengirim ke Cloud...")
            
            for tx in data_tertunda:
                id_tx, id_ptn, id_kios, vol, waktu, token, harga, pendapatan = tx
                
                waktu_str = waktu.strftime('%Y-%m-%d %H:%M:%S') if isinstance(waktu, datetime) else str(waktu)
                
                data_cloud = {
                    "id_transaksi": id_tx, 
                    "id_peternak": id_ptn,
                    "id_kios_penyetoran": id_kios,
                    "volume_liter": float(vol),
                    "waktu_setor": waktu_str,  
                    "token": token,
                    "harga_per_liter": float(harga) if harga else 7500,
                    "total_pendapatan": float(pendapatan) if pendapatan else (float(vol) * 7500)
                }
                
                try:
                    # Gunakan UPSERT (bukan sekadar insert) agar tidak error jika data ternyata sudah masuk
                    supabase.table("tb_transaksi").upsert(data_cloud).execute()
                    
                    # Jika tidak error (berarti sukses terkirim), ubah status di lokal menjadi TRUE
                    cur.execute("UPDATE tb_transaksi SET status_sinkron = TRUE WHERE id_transaksi = %s", (id_tx,))
                    conn.commit()
                    logging.info(f"[SYNC-SUSULAN] Transaksi #{id_tx} BERHASIL disusulkan ke Cloud!")
                    
                except Exception as e_cloud:
                    logging.warning(f"[SYNC-SUSULAN] Masih gagal kirim tx #{id_tx}. Internet mati? Keluar dari loop. Alasan {e_cloud}")
                    break
                    
            cur.close()
        except Exception as e:
            logging.error(f"[SYNC-SUSULAN] Error database lokal: {e}")
        finally:
            if conn:
                conn.close()

def jalankan_audit_integritas():
    while True:
        conn = None
        try:
            conn = psycopg2.connect(**DB_LOCAL)
            cur = conn.cursor()
            cur.execute("SELECT id_transaksi, id_peternak, id_kios_penyetoran, volume_liter, waktu_setor, token FROM tb_transaksi;")
            semua_transaksi = cur.fetchall()
            
            for tx in semua_transaksi:
                id_tx, id_ptn, id_kios, volume, waktu_setor, token_db = tx
                waktu_str = waktu_setor.strftime('%Y-%m-%d %H:%M:%S')
                token_kalkulasi_ulang = buat_token_sha256(id_ptn, id_kios, volume, waktu_str)
                
                if token_db != token_kalkulasi_ulang:
                    logging.warning(
                        f"[AUDIT] DETEKSI MANIPULASI DATA PADA TRANSAKSI #{id_tx}! "
                        f"Token hasil kalkulasi ({token_kalkulasi_ulang[:10]}...) tidak cocok dengan token database ({token_db[:10]}...)."
                    )
            cur.close()
        except Exception as e:
            logging.error(f"[AUDIT] Gagal melakukan audit data: {e}")
        finally:
            if conn:
                conn.close()
        time.sleep(10)

def main():
    noUrut = 1

    logging.info("Service TCP Hybrid (Lokal + Cloud) berjalan...")
    thread_audit = threading.Thread(target=jalankan_audit_integritas, daemon=True)
    thread_audit.start()
    logging.info("Sistem Pengecekan Integritas SHA-256 Aktif berkala (10s)...")

    thread_sync = threading.Thread(target=jalankan_sinkronisasi_susulan, daemon=True)
    thread_sync.start()
    logging.info("Sistem Sinkronisasi Susulan Cloud Aktif berkala (60s)...")

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        server_socket.bind((SERVER_IP, SERVER_PORT))
        server_socket.listen(5)
        logging.info(f"Mendengarkan sensor pada port {SERVER_PORT}...")
    except Exception as e:
        logging.error(f"Gagal bind port: {e}")
        return

    while True:
        try:
            client_socket, addr = server_socket.accept()
            data = client_socket.recv(2048)
            
            if data:
                try:
                    print(f"{noUrut}. Data Mentah (AES-256)   : {data}")
                    noUrut += 1

                    teks_hex = data.decode('utf-8').strip()
                    data_enkripsi_bytes = bytes.fromhex(teks_hex)
                    
                    cipher = AES.new(KUNCI_AES, AES.MODE_CBC, IV_AES)

                    awal_dekripsi_AES = time.perf_counter()
                    hasil_dekripsi_bytes = unpad(cipher.decrypt(data_enkripsi_bytes), AES.block_size)
                    akhir_dekripsi_AES = time.perf_counter()
                    hasil_dekripsi = hasil_dekripsi_bytes.decode('utf-8')
                    waktu_dekripsi_AES = (akhir_dekripsi_AES - awal_dekripsi_AES) * 1000
                    
                    print(f"Pesan Terdekripsi       : {hasil_dekripsi}")
                    print(f"Waktu Dekripsi          : {waktu_dekripsi_AES:.4f} ms")
                    # =========================================================

                    # 2. Parsing Format: "UID,ID_KIOS,VOLUME"
                    parts = hasil_dekripsi.split('-')
                    if len(parts) == 4:
                        id_kios, uid_rfid, volume, waktu_node = parts
                        
                        # 3. Eksekusi penyimpanan
                        simpan_dan_sinkron(
                            uid_rfid.strip(), 
                            id_kios.strip(), 
                            float(volume),
                            waktu_node.strip()
                        )
                    else:
                        logging.warning(f"Format data tidak dikenali: {hasil_dekripsi}")

                except ValueError as ve:
                    logging.warning(f"Gagal dekripsi (Kunci salah atau Padding rusak): {ve}")
                except Exception as e:
                    logging.warning(f"Gagal dekripsi/parsing secara umum: {e}")
            
            client_socket.close()
        except Exception as e:
            logging.error(f"Error pada loop utama TCP: {e}")
            time.sleep(1)

if __name__ == "__main__":
    main()
