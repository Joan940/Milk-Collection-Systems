// import 'dart:io';
import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:supabase_flutter/supabase_flutter.dart';
import 'package:intl/intl.dart';
import 'package:intl/date_symbol_data_local.dart';

import 'package:pdf/pdf.dart';
import 'package:pdf/widgets.dart' as pw;
import 'package:printing/printing.dart';

// KELAS HISTORY LIST UNTUK MENAMPILKAN RIWAYAT DAN CETAK PDF
class HistoryList extends StatelessWidget {
  final String namaPeternak;
  final String idPeternak;

  const HistoryList({
    super.key, 
    required this.namaPeternak,
    required this.idPeternak,
  });

  @override
  Widget build(BuildContext context) {
    const Color warnaUtama = Color(0xFF523D35);
    final ScrollController scrollController = ScrollController();
    final supabase = Supabase.instance.client;
    
    // MENYIAPKAN VARIABEL WAKTU UNTUK FILTER KHUSUS BULAN INI
    final DateTime hariIni = DateTime.now();
    // final DateTime awalBulanIni = DateTime(hariIni.year, hariIni.month, 1);

    return Container(
      width: double.infinity,
      padding: const EdgeInsets.fromLTRB(25, 25, 25, 20),
      // MENGATUR DESAIN KOTAK UTAMA RIWAYAT
      decoration: BoxDecoration(
        color: Colors.white,
        borderRadius: BorderRadius.circular(25),
        boxShadow: [
          BoxShadow(
            color: Colors.black.withValues(alpha: 0.03),
            blurRadius: 15,
            offset: const Offset(0, 8),
          )
        ],
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            "Riwayat Penyetoran",
            style: GoogleFonts.poppins(
              fontSize: 17,
              fontWeight: FontWeight.bold,
              color: warnaUtama,
            ),
          ),
          const SizedBox(height: 15),
          
          // MENGAMBIL DATA TRANSAKSI DARI SUPABASE SECARA REALTIME
          StreamBuilder<List<Map<String, dynamic>>>(
            stream: supabase
                .from('tb_transaksi')
                .stream(primaryKey: ['id_transaksi'])
                .eq('id_peternak', idPeternak)
                .order('waktu_setor', ascending: false),
            builder: (context, snapshot) {
              if (snapshot.connectionState == ConnectionState.waiting) {
                return const Center(child: CircularProgressIndicator());
              }

              if (snapshot.hasError) {
                return Center(
                  child: Text(
                    "Gagal memuat data riwayat",
                    style: GoogleFonts.poppins(fontSize: 12, color: Colors.red),
                  ),
                );
              }

              final rawData = snapshot.data ?? [];

              // MENYARING DATA MENGGUNAKAN DART UNTUK MENDAPATKAN TRANSAKSI BULAN INI SAJA
              final listData = rawData.where((item) {
                if (item['waktu_setor'] == null) return false;
                DateTime waktuServer = DateTime.parse(item['waktu_setor']);
                return waktuServer.year == hariIni.year && waktuServer.month == hariIni.month;
              }).toList(); 

              if (listData.isEmpty) {
                return Center(
                  child: Text(
                    "Belum ada setoran bulan ini.",
                    style: GoogleFonts.poppins(
                      fontSize: 12,
                      color: Colors.grey,
                      fontStyle: FontStyle.italic,
                    ),
                  ),
                );
              }

              // MEMBATASI TINGGI KOTAK AGAR BISA DISUKAI SCROLL MANDIRI
              return ConstrainedBox(
                constraints: const BoxConstraints(
                  maxHeight: 330, 
                ),
                child: Scrollbar(
                  controller: scrollController,
                  thumbVisibility: true,
                  child: ListView.separated(
                    controller: scrollController,
                    shrinkWrap: true,
                    // MENGIZINKAN KOTAK INI DISUBLIM SCROLL MESKIPUN ADA DI DALAM SCROLL UTAMA
                    physics: const BouncingScrollPhysics(),
                    padding: const EdgeInsets.only(right: 15),
                    itemCount: listData.length,
                    separatorBuilder: (context, index) => Divider(
                      color: Colors.grey.withValues(alpha: 0.1),
                      height: 25,
                    ),
                    itemBuilder: (context, index) {
                      final item = listData[index];
                      
                      // PENGAMAN JIKA TANGGAL DARI DATABASE KOSONG
                      if (item['waktu_setor'] == null) return const SizedBox.shrink();

                      try {
                        // PARSING TANGGAL DAN ANGKA DENGAN AMAN
                        DateTime waktuServer = DateTime.parse(item['waktu_setor']);
                        String tgl = DateFormat('EEEE, dd MMM', 'id').format(waktuServer);
                        String jam = DateFormat('HH:mm').format(waktuServer);
                        double volume = (item['volume_liter'] ?? 0).toDouble();

                        // MENAMPILKAN BARIS TEKS TANGGAL DAN VOLUME SUSU
                        return Row(
                          mainAxisAlignment: MainAxisAlignment.spaceBetween,
                          children: [
                            Column(
                              crossAxisAlignment: CrossAxisAlignment.start,
                              children: [
                                Text(
                                  tgl,
                                  style: GoogleFonts.poppins(
                                    fontSize: 12,
                                    fontWeight: FontWeight.w600,
                                    color: warnaUtama,
                                  ),
                                ),
                                Text(
                                  jam,
                                  style: const TextStyle(fontSize: 10, color: Colors.grey),
                                ),
                              ],
                            ),
                            Text(
                              "${volume.toStringAsFixed(1)} Ltr",
                              style: GoogleFonts.poppins(
                                fontSize: 13,
                                fontWeight: FontWeight.bold,
                                color: warnaUtama,
                              ),
                            ),
                          ],
                        );
                      } catch (e) {
                        return const SizedBox.shrink();
                      }
                    },
                  ),
                ),
              );
            },
          ),
          
          const SizedBox(height: 15),
          
          // TOMBOL DOWNLOAD UNTUK PDF RIWAYAT DAN STRUK GAJI
          Row(
            children: [
              Expanded(
                child: ElevatedButton.icon(
                  style: ElevatedButton.styleFrom(
                    backgroundColor: warnaUtama,
                    foregroundColor: Colors.white,
                    padding: const EdgeInsets.symmetric(vertical: 12),
                    shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10))
                  ),
                  icon: const Icon(Icons.table_view, size: 18),
                  label: Text("Riwayat", style: GoogleFonts.poppins(fontSize: 12)),
                  onPressed: () => _downloadRiwayatPdf(context),
                ),
              ),
              const SizedBox(width: 10),
              Expanded(
                child: ElevatedButton.icon(
                  style: ElevatedButton.styleFrom(
                    backgroundColor: Colors.white,
                    foregroundColor: warnaUtama,
                    padding: const EdgeInsets.symmetric(vertical: 12),
                    side: const BorderSide(color: Color(0xFF523D35)),
                    shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(10))
                  ),
                  icon: const Icon(Icons.picture_as_pdf, size: 18),
                  label: Text("Struk", style: GoogleFonts.poppins(fontSize: 12)),
                  onPressed: () => _downloadStrukPDF(context),
                ),
              ),
            ],
          )
        ],
      ),
    );
  }

  // FUNGSI UNTUK MENCETAK DAN MENGUNDUH PDF RIWAYAT
  Future<void> _downloadRiwayatPdf(BuildContext context) async {
    await initializeDateFormatting('id_ID', null);

    try {
      final DateTime hariIni = DateTime.now();
      final DateTime awalBulanIni = DateTime(hariIni.year, hariIni.month, 1);

      // MENGAMBIL DATA TRANSAKSI KHUSUS BULAN INI UNTUK DICETAK
      final response = await Supabase.instance.client
          .from('tb_transaksi')
          .select('*')
          .eq('id_peternak', idPeternak)
          .gte('waktu_setor', awalBulanIni.toIso8601String())
          .order('waktu_setor', ascending: true);

      final pdf = pw.Document();
      List<List<dynamic>> rows = [];
      rows.add(["Tanggal", "Jam", "ID Transaksi", "Volume (Ltr)"]);

      double totalVolumeBulanIni = 0.0;

      // MENGISI DATA KE DALAM BARIS TABEL PDF
      for(var data in response) {
        if (data['waktu_setor'] == null) continue;

        DateTime waktuServer = DateTime.parse(data['waktu_setor']);
        String tgl = DateFormat('dd MMM yyyy', 'id').format(waktuServer);
        String jam = DateFormat('HH:mm').format(waktuServer);
        double volume = (data['volume_liter'] ?? 0).toDouble();

        totalVolumeBulanIni += volume;

        rows.add([
          tgl,
          jam,
          data['id_transaksi']?.toString() ?? '-', 
          "${volume.toStringAsFixed(1)} Ltr"       
        ]);
      }

      // MEMBUAT DOKUMEN HALAMAN MULTIPAGE AGAR TABEL PANJANG TIDAK TERPOTONG
      pdf.addPage(
        pw.MultiPage( 
          pageFormat: PdfPageFormat.a4,
          build: (pw.Context ctx) {
            pw.Widget buildCell(String text, {required int flex, bool isHeader = false, pw.Alignment align = pw.Alignment.center, bool rightBorder = false}) {
              return pw.Expanded(
                flex: flex,
                child: pw.Container(
                  padding: const pw.EdgeInsets.symmetric(vertical: 6, horizontal: 5),
                  decoration: pw.BoxDecoration(
                    border: rightBorder ? const pw.Border(right: pw.BorderSide(color: PdfColors.black, width: 1)) : null,
                  ),
                  alignment: align,
                  child: pw.Text(
                    text,
                    style: pw.TextStyle(fontWeight: isHeader ? pw.FontWeight.bold : pw.FontWeight.normal),
                  ),
                ),
              );
            }
            
            // DESAIN HEADER DAN BENTUK TABEL PDF
            return [
              pw.Text("KOPERASI UNIT DESA KOPENG", style: pw.TextStyle(fontSize: 20, fontWeight: pw.FontWeight.bold, color: PdfColors.brown800)),
              pw.Text("Laporan Riwayat Setoran Susu", style: const pw.TextStyle(fontSize: 12, color: PdfColors.grey700)),
              pw.Divider(thickness: 2, color: PdfColors.brown800),
              pw.SizedBox(height: 20),

              pw.Text("Nama Peternak : $namaPeternak", style: pw.TextStyle(fontWeight: pw.FontWeight.bold)),
              pw.Text("Periode              : Bulan ${DateFormat('MMMM yyyy', 'id').format(hariIni)}", style: pw.TextStyle(fontWeight: pw.FontWeight.bold)),
              pw.SizedBox(height: 20),

              pw.TableHelper.fromTextArray(
                border: pw.TableBorder.all(color: PdfColors.black, width: 1),
                headerStyle: pw.TextStyle(fontWeight: pw.FontWeight.bold, color: PdfColors.black),
                columnWidths: {
                  0: const pw.FlexColumnWidth(4),
                  1: const pw.FlexColumnWidth(3),
                  2: const pw.FlexColumnWidth(4),
                  3: const pw.FlexColumnWidth(4),
                },
                cellAlignments: {
                  0: pw.Alignment.center,
                  1: pw.Alignment.center,
                  2: pw.Alignment.center,
                  3: pw.Alignment.centerRight,
                },
                data: rows,
              ),

              pw.Container(
                decoration: const pw.BoxDecoration(
                  border: pw.Border(
                    left: pw.BorderSide(color: PdfColors.black, width: 1),
                    right: pw.BorderSide(color: PdfColors.black, width: 1),
                    bottom: pw.BorderSide(color: PdfColors.black, width: 1),
                  )
                ),
                child: pw.Row(
                  children: [
                    buildCell("TOTAL AKUMULASI", flex: 11, isHeader: true, rightBorder: true, align: pw.Alignment.centerRight),
                    buildCell("${totalVolumeBulanIni.toStringAsFixed(1)} Ltr", flex: 4, isHeader: true, rightBorder: false, align: pw.Alignment.centerRight),
                  ]
                )
              ),
            ];
          },
        ),
      );

      // MEMBUKA MENU BAGIKAN PDF UNTUK MENGHINDARI ERROR IZIN PENYIMPANAN ANDROID
      await Printing.sharePdf(
        bytes: await pdf.save(),
        filename: 'Riwayat_Susu_${namaPeternak.replaceAll(' ', '_')}.pdf',
      );

    } catch (e) {
      if (!context.mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text("Gagal mendownload riwayat: $e"), backgroundColor: Colors.red),
      );
    }
  }

  // FUNGSI UNTUK MENCETAK DAN MENGUNDUH PDF STRUK GAJI
  Future<void> _downloadStrukPDF(BuildContext context) async {
    await initializeDateFormatting('id_ID', null);

    try {
      final DateTime hariIni = DateTime.now();
      final DateTime awalBulanIni = DateTime(hariIni.year, hariIni.month, 1);

      // MENGAMBIL DATA HARGA DAN PENDAPATAN DARI DATABASE
      final response = await Supabase.instance.client
          .from('tb_transaksi')
          .select('volume_liter, harga_per_liter, total_pendapatan, waktu_setor')
          .eq('id_peternak', idPeternak)
          .gte('waktu_setor', awalBulanIni.toIso8601String())
          .order('waktu_setor', ascending: true);
      
      double totalVolume = 0.0;
      double totalPendapatan = 0.0;
      double hargaPerLiterHariIni = 0.0;

      // MENGAKUMULASI TOTAL PENDAPATAN SECARA LANGSUNG DARI DATABASE TANPA DIKALI MANUAL
      for (var row in response) {
        totalVolume += (row['volume_liter'] ?? 0).toDouble();
        totalPendapatan += (row['total_pendapatan'] ?? 0).toDouble();
        
        if (row['harga_per_liter'] != null) {
          hargaPerLiterHariIni = row['harga_per_liter'].toDouble();
        }
      }

      final formatRupiah = NumberFormat.currency(locale: 'id_ID', symbol: 'Rp ', decimalDigits: 0);
      final pdf = pw.Document();

      // MEMBUAT DOKUMEN PDF STRUK GAJI
      pdf.addPage(
        pw.Page(
          pageFormat: PdfPageFormat.a4,
          build: (pw.Context ctx) {
            pw.Widget buildCell(String text, {required int flex, bool isHeader = false, pw.Alignment align = pw.Alignment.center, bool rightBorder = false}) {
              return pw.Expanded(
                flex: flex,
                child: pw.Container(
                  padding: const pw.EdgeInsets.symmetric(vertical: 6, horizontal: 5),
                  decoration: pw.BoxDecoration(
                    border: rightBorder ? const pw.Border(right: pw.BorderSide(color: PdfColors.black, width: 1)) : null,
                  ),
                  alignment: align,
                  child: pw.Text(
                    text,
                    style: pw.TextStyle(fontWeight: isHeader ? pw.FontWeight.bold : pw.FontWeight.normal),
                  ),
                ),
              );
            }

            // MENGATUR DESAIN TABEL STRUK GAJI
            return pw.Column(
              crossAxisAlignment: pw.CrossAxisAlignment.start,
              children: [
                pw.Text("KOPERASI UNIT DESA KOPENG", 
                    style: pw.TextStyle(fontSize: 20, fontWeight: pw.FontWeight.bold, color: PdfColors.brown800)),
                pw.Text("Bukti Pembayaran Setoran Susu Digital (Struk Gaji)", 
                    style: const pw.TextStyle(fontSize: 12, color: PdfColors.grey700)),
                pw.Divider(thickness: 2, color: PdfColors.brown800),
                pw.SizedBox(height: 20),

                pw.Text("Nama Peternak : $namaPeternak", style: pw.TextStyle(fontWeight: pw.FontWeight.bold)),
                pw.Text("Periode Gaji      : Bulan ${DateFormat('MMMM yyyy', 'id').format(hariIni)}", style: pw.TextStyle(fontWeight: pw.FontWeight.bold)),
                pw.Text("Tanggal Cetak   : ${DateFormat('dd MMMM yyyy', 'id').format(hariIni)}", style: pw.TextStyle(fontWeight: pw.FontWeight.bold)),
                pw.SizedBox(height: 30),

                pw.Container(
                  decoration: pw.BoxDecoration(
                    border: pw.Border.all(color: PdfColors.black, width: 1),
                  ),
                  child: pw.Column(
                    children: [
                      pw.Row(
                        children: [
                          buildCell('Keterangan', flex: 4, isHeader: true, rightBorder: true),
                          buildCell('Kuantitas', flex: 3, isHeader: true, rightBorder: true),
                          buildCell('Harga / Ltr', flex: 3, isHeader: true, rightBorder: true),
                          buildCell('Subtotal', flex: 3, isHeader: true, rightBorder: false),
                        ],
                      ),
                      pw.Divider(height: 0, thickness: 1, color: PdfColors.black),

                      pw.Row(
                        children: [
                          buildCell('Akumulasi Volume Susu', flex: 4, rightBorder: true),
                          buildCell(totalVolume.toStringAsFixed(1), flex: 3, rightBorder: true),
                          buildCell(formatRupiah.format(hargaPerLiterHariIni), flex: 3, rightBorder: true), 
                          buildCell(formatRupiah.format(totalPendapatan), flex: 3, rightBorder: false),
                        ],
                      ),
                      pw.Divider(height: 0, thickness: 1, color: PdfColors.black),

                      pw.Row(
                        children: [
                          buildCell('Total', flex: 10, isHeader: true, rightBorder: true),
                          buildCell(formatRupiah.format(totalPendapatan), flex: 3, isHeader: true, rightBorder: false),
                        ],
                      ),
                    ],
                  ),
                ),
                
                pw.Spacer(),
                
                // MENAMPILKAN KOLOM TANDA TANGAN
                pw.Row(
                  mainAxisAlignment: pw.MainAxisAlignment.spaceBetween,
                  children: [
                    pw.Column(
                      children: [
                        pw.Text("Mengetahui,"),
                        pw.Text("Pengelola TPS"),
                        pw.SizedBox(height: 60),
                        pw.Text("( _________________ )")
                      ]
                    ),
                    pw.Column(
                      children: [
                        pw.Text("Penerima Manfaat,"),
                        pw.Text("Peternak"),
                        pw.SizedBox(height: 60),
                        pw.Text("( $namaPeternak )")
                      ]
                    )
                  ]
                )
              ],
            );
          },
        ),
      );

      // MEMBUKA MENU BAGIKAN PDF UNTUK STRUK GAJI
      await Printing.sharePdf(
        bytes: await pdf.save(),
        filename: 'Struk_Gaji_${namaPeternak.replaceAll(' ', '_')}.pdf',
      );

    } catch (e) {
      if (!context.mounted) return;
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text("Gagal mendownload struk: $e")),
      );
    }
  }
}