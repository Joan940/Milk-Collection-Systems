import psycopg2
from psycopg2.extras import RealDictCursor

DB_LOCAL = {
    "host": "127.0.0.1",
    "database": "Data_Peternak",
    "user": "postgres",
    "password": "kipaza08",
    "port": "5432"
}

def cekLoginAdmin(input_user, input_pass):
    conn = None
    try:
        conn = psycopg2.connect(**DB_LOCAL)
        cur = conn.cursor()
        
        query = "SELECT id_admin FROM tb_admin WHERE username = %s AND password = %s;"
        cur.execute(query, (input_user, input_pass))
        result = cur.fetchone()
        
        if result:
            return True
        else:
            return False
            
    except Exception as e:
        print(f"Error Database Login: {e}")
        return False
    finally:
        if conn:
            cur.close()
            conn.close()

def getNamaPeternak(nama):
    conn = None
    try:
        conn = psycopg2.connect(**DB_LOCAL)
        cur = conn.cursor(cursor_factory=RealDictCursor)
        
        query = "SELECT * FROM tb_peternak WHERE nama_peternak ILIKE %s;"
        cur.execute(query, (f"%{nama}%",))
        
        data     = cur.fetchall()
        dataNama = [baris['nama_peternak'] for baris in data]
        return dataNama
    except Exception as e:
        print(f"Error getDataPeternak: {e}")
        return []
    finally:
        if conn:
            cur.close()
            conn.close()

def getRiwayatSetoranPeternak(nama):
    conn = None
    try:
        conn = psycopg2.connect(**DB_LOCAL)
        cur = conn.cursor(cursor_factory=RealDictCursor)
        
        query = """
            SELECT 
                t.id_transaksi, 
                t.volume_liter, 
                t.waktu_setor, 
                t.token,
                t.total_pendapatan, 
                p.nama_peternak, 
                p.alamat,
                t.id_kios
            FROM tb_transaksi t
            INNER JOIN tb_peternak p ON t.id_peternak = p.id_peternak
            WHERE p.nama_peternak ILIKE %s
            ORDER BY t.id_transaksi DESC;
        """
        cur.execute(query, (f"%{nama}%",))
        
        data = cur.fetchall()
        return data
    except Exception as e:
        print(f"Error getRiwayatSetoranPeternak: {e}")
        return []
    finally:
        if conn:
            cur.close()
            conn.close()

def getRiwayatWaktuDanVolume(nama):
    conn = None
    try:
        conn = psycopg2.connect(**DB_LOCAL)
        cur = conn.cursor(cursor_factory=RealDictCursor)
        
        query = """
            SELECT t.volume_liter, t.waktu_setor 
            FROM tb_transaksi t
            INNER JOIN tb_peternak p ON t.id_peternak = p.id_peternak
            WHERE p.nama_peternak ILIKE %s
            ORDER BY t.waktu_setor DESC;
        """
        cur.execute(query, (f"%{nama}%",))
        
        data = cur.fetchall()
        return data
        
    except Exception as e:
        print(f"Error getRiwayatWaktuDanVolume: {e}")
        return []
    finally:
        if conn:
            cur.close()
            conn.close()

def getRiwayatSetoranTPS(id_tps):
    conn = None
    try:
        conn = psycopg2.connect(**DB_LOCAL)
        cur = conn.cursor(cursor_factory=RealDictCursor)
        
        query = """
            SELECT 
                t.id_transaksi, 
                t.volume_liter, 
                t.total_pendapatan,
                t.waktu_setor, 
                p.nama_peternak, 
                t.id_kios
            FROM tb_transaksi t
            INNER JOIN tb_peternak p ON t.id_peternak = p.id_peternak
            WHERE t.id_kios = %s
            ORDER BY t.id_transaksi DESC;
        """
        cur.execute(query, (id_tps,))
        return cur.fetchall()
        
    except Exception as e:
        print(f"Error getRiwayatSetoranTPS: {e}")
        return []
    finally:
        if conn:
            cur.close()
            conn.close()

def getProfilPeternak(nama):
    conn = None
    try:
        conn = psycopg2.connect(**DB_LOCAL)
        cur = conn.cursor(cursor_factory=RealDictCursor)
        
        query = """
            SELECT nama_peternak, alamat 
            FROM tb_peternak 
            WHERE nama_peternak ILIKE %s
            LIMIT 1;
        """
        cur.execute(query, (f"%{nama}%",))
        
        data = cur.fetchone()
        return data if data else {}
        
    except Exception as e:
        print(f"Error getProfilPeternak: {e}")
        return {}
    finally:
        if conn:
            cur.close()
            conn.close()

def getDashboardTpsHariIni(tps_id):
    conn = None
    data_dashboard = {
        "total_volume": 0.0,
        "pendapatan": 0.0,
        "riwayat": []
    }
    
    try:
        # Sesuaikan dengan variabel koneksi DB-mu
        conn = psycopg2.connect(**DB_LOCAL) 
        cur = conn.cursor(cursor_factory=RealDictCursor)
        
        query_kpi = """
            SELECT SUM(volume) as total_vol, SUM(pendapatan) as total_rp 
            FROM tb_setoran 
            WHERE id_tps = %s AND DATE(waktu) = CURRENT_DATE;
        """
        cur.execute(query_kpi, (tps_id,))
        kpi = cur.fetchone()
        
        if kpi['total_vol']: data_dashboard["total_volume"] = float(kpi['total_vol'])
        if kpi['total_rp']: data_dashboard["pendapatan"] = float(kpi['total_rp'])
        
        query_riwayat = """
            SELECT TO_CHAR(waktu, 'Day, DD Mon') as tanggal, 
                   TO_CHAR(waktu, 'HH24:MI') as jam, 
                   volume 
            FROM tb_setoran 
            WHERE id_tps = %s AND DATE(waktu) = CURRENT_DATE
            ORDER BY waktu DESC;
        """
        cur.execute(query_riwayat, (tps_id,))
        data_dashboard["riwayat"] = cur.fetchall()
        
        return data_dashboard

    except Exception as e:
        print(f"Error getDashboardTpsHariIni: {e}")
        return data_dashboard
    finally:
        if conn:
            cur.close()
            conn.close()

def getHargaHarianLokal():
    conn = None
    try:
        conn = psycopg2.connect(**DB_LOCAL)
        cur = conn.cursor()
        cur.execute("SELECT harga_harian FROM pengaturan_sistem ORDER BY id DESC LIMIT 1;")
        result = cur.fetchone()
        return int(result[0]) if result else 7300
    except Exception as e:
        print(f"Error getHargaHarianLokal: {e}")
        return 7500
    finally:
        if conn:
            cur.close()
            conn.close()

def updateHargaHarianLokal(harga_baru):
    conn = None
    try:
        conn = psycopg2.connect(**DB_LOCAL)
        cur = conn.cursor()

        query = """
            INSERT INTO pengaturan_sistem (harga_harian, tanggal_berlaku) 
            VALUES (%s, CURRENT_DATE);
        """
        cur.execute(query, (harga_baru,))
        conn.commit()
        return True
    except Exception as e:
        print(f"Error updateHargaHarianLokal: {e}")
        return False
    finally:
        if conn:
            cur.close()
            conn.close()

# ==========================================
# CONTOH PENGGUNAAN / TESTING
# ==========================================
# if __name__ == "__main__":
#     print("--- Testing Get Total Data ---")
#     print(f"Total Transaksi: {getTotalData()}")

#     print("\n--- Testing Get Data Peternak ---")
#     peternak = getDataPeternak("joan")
#     for p in peternak:
#         print(p)

#     print("\n--- Testing Get Riwayat Setoran ---")
#     riwayat = getRiwayatSetoranPeternak("joan")
#     for r in riwayat:
#         # Mengakses data seperti dictionary Python biasa
#         print(f"[{r['waktu_setor']}] {r['nama_peternak']} setor {r['volume_liter']} Liter (Token: {r['token'][:10]}...)")

