import 'package:flutter/material.dart';
import 'package:fl_chart/fl_chart.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:supabase_flutter/supabase_flutter.dart';

// KELAS GRAFIK CHART UNTUK MENAMPILKAN DATA SENSOR SECARA VISUAL
class GrafikChart extends StatelessWidget {
  final String idPeternak;
  const GrafikChart({super.key, required this.idPeternak});

  @override
  Widget build(BuildContext context) {
    const Color warnaUtama = Color(0xFF523D35);
    final supabase = Supabase.instance.client;

    return Container(
      width: double.infinity,
      padding: const EdgeInsets.all(25),
      // MENGATUR DESAIN KOTAK GRAFIK
      decoration: BoxDecoration(
        color: Colors.white,
        borderRadius: BorderRadius.circular(25),
        boxShadow: [
          BoxShadow(
            color: Colors.black.withValues(alpha: 0.03),
            blurRadius: 15, 
            offset: const Offset(0, 8)
          )
        ],
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            "Dashboard Monitoring", 
            style: GoogleFonts.poppins(fontSize: 18, fontWeight: FontWeight.bold, color: warnaUtama)
          ),
          const Text(
            "Data sensor LiDAR", 
            style: TextStyle(color: Colors.grey, fontSize: 11)
          ),
          const SizedBox(height: 20),
          
          // MENGAMBIL DATA TRANSAKSI TERAKHIR DARI SUPABASE UNTUK DIBUAT GRAFIK
          SizedBox(
            height: 160,
            child: StreamBuilder<List<Map<String, dynamic>>>(
              stream: supabase
                  .from('tb_transaksi')
                  .stream(primaryKey: ['id_transaksi'])
                  .eq('id_peternak', idPeternak)
                  .order('waktu_setor', ascending: true)
                  .limit(7),
              builder: (context, snapshot) {
                // MENANGANI LOADING DAN ERROR GRAFIK
                if (snapshot.connectionState == ConnectionState.waiting) {
                  return const Center(child: CircularProgressIndicator());
                }

                if (snapshot.hasError) {
                  return Center(
                    child: Text(
                      "Gagal memuat grafik", 
                      style: TextStyle(fontSize: 10, color: Colors.red[300])
                    ),
                  );
                }

                final listData = snapshot.data ?? [];

                if (listData.isEmpty) {
                  return Center(
                    child: Text(
                      "Belum ada data sensor masuk.",
                      style: GoogleFonts.poppins(fontSize: 12, color: Colors.grey, fontStyle: FontStyle.italic),
                    ),
                  );
                }

                // PARSING DATA MENJADI TITIK GRAFIK DENGAN PENGAMAN TRY PARSE
                final spots = listData.asMap().entries.map((e) {
                  double yValue = double.tryParse(e.value['volume_liter'].toString()) ?? 0.0;
                  return FlSpot(e.key.toDouble(), yValue);
                }).toList();

                // MENCARI NILAI TERTINGGI UNTUK MENGATUR BATAS ATAS GRAFIK SECARA DINAMIS ATAU MENGIKUTI MAKSIMAL WADAH 20 LITER
                double maxVolume = 0;
                for (var spot in spots) {
                  if (spot.y > maxVolume) {
                    maxVolume = spot.y;
                  }
                }
                
                double chartMaxY = maxVolume > 20 ? maxVolume + 5 : 25;

                return LineChart(
                  LineChartData(
                    minY: 0, 
                    maxY: chartMaxY, 
                    // KONFIGURASI TAMPILAN GARIS DAN GRID GRAFIK
                    gridData: FlGridData(
                      show: true, 
                      horizontalInterval: 5, 
                      drawVerticalLine: false, 
                      getDrawingHorizontalLine: (v) => FlLine(
                        dashArray: [5, 5], 
                        color: Colors.grey.withValues(alpha: 0.1)
                      )
                    ),
                    titlesData: const FlTitlesData(show: false),
                    borderData: FlBorderData(show: false),
                    lineBarsData: [
                      LineChartBarData(
                        spots: spots,
                        isCurved: true, 
                        color: warnaUtama,
                        barWidth: 3,
                        dotData: const FlDotData(show: true),
                        belowBarData: BarAreaData(
                          show: true, 
                          color: warnaUtama.withValues(alpha: 0.05)
                        ),
                      ),
                    ],
                  ),
                );
              },
            ),
          ),
        ],
      ),
    );
  }
}