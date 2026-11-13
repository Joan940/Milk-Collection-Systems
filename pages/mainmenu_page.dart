import 'package:flutter/material.dart';
import 'package:flutter_svg/flutter_svg.dart';
import 'package:hp_application_1/main.dart';
import 'package:intl/intl.dart';
import '../widgets/grafik_card.dart';
import '../widgets/stat_card.dart';
import '../widgets/history_list.dart';

// KELAS MAIN MENU PAGE SEBAGAI HALAMAN UTAMA DASBOR
class MainmenuPage extends StatelessWidget {
  final String namaUserLogin;
  final String idUserLogin;
  const MainmenuPage({
    super.key, 
    required this.namaUserLogin,
    required this.idUserLogin,
  });

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color(0xFFEFEFE9),
      body: SafeArea(
        // MENGGUNAKAN SCROLL AGAR HALAMAN BISA DIGESER DARI ATAS SAMPAI BAWAH
        child: SingleChildScrollView(
          child: Padding(
            padding: const EdgeInsets.symmetric(horizontal: 25.0),
            child: Column(
              children: [
                const SizedBox(height: 30),
                // MENAMPILKAN KOMPONEN GRAFIK CHART
                GrafikChart(idPeternak: idUserLogin),
                const SizedBox(height: 20),
                
                // MENGAMBIL DATA UNTUK MENGHITUNG KOTAK STATISTIK BULAN INI
                Row(
                  children: [
                    Expanded(
                      child: StreamBuilder<List<Map<String, dynamic>>>(
                        stream: supabase
                            .from('tb_transaksi')
                            .stream(primaryKey: ['id_transaksi'])
                            .eq('id_peternak', idUserLogin),
                        builder: (context, snapshot) {
                          if (snapshot.connectionState == ConnectionState.waiting) {
                            return const Center(child: CircularProgressIndicator());
                          }

                          if (snapshot.hasError) {
                            return const Center(child: Text("Gagal memuat data"));
                          }

                          final dataTransaksi = snapshot.data ?? [];
                          double totalVolume = 0.0;
                          double totalPendapatan = 0.0;

                          final now = DateTime.now();
                          
                          // MENGHITUNG TOTAL VOLUME DAN PENDAPATAN KHUSUS BULAN INI BESERTA PENGAMAN PARSING ANGKA DAN TANGGAL
                          for (var row in dataTransaksi) {
                            if (row['waktu_setor'] != null) {
                              DateTime? tanggalTransaksi = DateTime.tryParse(row['waktu_setor'].toString());
                            
                              if (tanggalTransaksi != null && tanggalTransaksi.year == now.year && tanggalTransaksi.month == now.month) {
                                totalVolume += double.tryParse(row['volume_liter'].toString()) ?? 0.0;
                                totalPendapatan += double.tryParse(row['total_pendapatan'].toString()) ?? 0.0;
                              }
                            }
                          }

                          final formatRupiah = NumberFormat.currency(
                            locale: 'id_ID',
                            symbol: '',
                            decimalDigits: 2
                          );

                          return Row(
                            children: [
                              // MENAMPILKAN KARTU STATISTIK TOTAL VOLUME
                              Expanded(
                                child: StatCard(
                                  title: "Total Volume Bulan Ini", 
                                  value: snapshot.connectionState == ConnectionState.waiting 
                                    ? "..." 
                                    : totalVolume.toStringAsFixed(1), 
                                  unit: "Ltr", 
                                  iconWidget: const Icon(
                                    Icons.water_drop,
                                    color: Color(0xFF523D35),
                                  )
                                )
                              ),

                              const SizedBox(width: 20),

                              // MENAMPILKAN KARTU STATISTIK TOTAL PENDAPATAN
                              Expanded(
                                child: StatCard(
                                  title: "Pendapatan Bulan Ini", 
                                  value: snapshot.connectionState == ConnectionState.waiting 
                                    ? "..." 
                                    : formatRupiah.format(totalPendapatan).trim(), 
                                  unit: "Rp",
                                  iconWidget: SvgPicture.asset(
                                    'assets/icons/MONEY.svg',
                                    height: 22.0,
                                    width: 22.0,
                                    colorFilter: const ColorFilter.mode(Color(0xFF523D35), BlendMode.srcIn),
                                  ),
                                )
                              ),
                            ],
                          );
                        },
                      ),
                    ),
                  ],
                ),

                const SizedBox(height: 20),
                
                // MENAMPILKAN KOMPONEN HISTORY LIST TANPA WIDGET EXPANDED AGAR TIDAK ERROR RENDER FLEX
                HistoryList(namaPeternak: namaUserLogin, idPeternak: idUserLogin),
                
                const SizedBox(height: 25),
              ],
            ),
          ),
        ),
      ),
    );
  }
}