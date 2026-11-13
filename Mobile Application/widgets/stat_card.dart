import 'package:flutter/material.dart';
import 'package:google_fonts/google_fonts.dart';

// KELAS STAT CARD UNTUK MENAMPILKAN KOTAK STATISTIK DI DASHBOARD
class StatCard extends StatelessWidget {
  final String title;
  final String value;
  final String unit;
  final Widget iconWidget;

  const StatCard({
    super.key,
    required this.title,
    required this.value,
    required this.unit,
    required this.iconWidget,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(20),
      // MENGATUR TAMPILAN KOTAK DAN BAYANGAN LATAR BELAKANG
      decoration: BoxDecoration(
        color: Colors.white,
        borderRadius: BorderRadius.circular(20),
        boxShadow: [
          BoxShadow(
            color: Colors.black.withValues(alpha: 0.03), 
            blurRadius: 15,
            offset: const Offset(0, 8),
          ),
        ],
      ),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          // MENAMPILKAN IKON 
          iconWidget,
          const SizedBox(height: 12),
          
          // MENGATUR TEKS JUDUL DAN MENCEGAH TEKS OVERFLOW JIKA TERLALU PANJANG
          Text(
            title, 
            maxLines: 1,
            overflow: TextOverflow.ellipsis,
            style: GoogleFonts.poppins(fontSize: 11, color: Colors.grey[600], fontWeight: FontWeight.w500)
          ),
          
          const SizedBox(height: 4),
          Row(
            crossAxisAlignment: CrossAxisAlignment.baseline,
            textBaseline: TextBaseline.alphabetic,
            children: [
              // MENGGUNAKAN FLEXIBLE AGAR ANGKA BESAR TIDAK MENABRAK SATUAN
              Flexible(
                child: Text(
                  value, 
                  maxLines: 1,
                  overflow: TextOverflow.ellipsis, 
                  style: GoogleFonts.poppins(
                    fontSize: 20, 
                    fontWeight: FontWeight.bold, 
                    color: const Color(0xFF523D35)
                  )
                ),
              ),
              const SizedBox(width: 4),
              // MENAMPILKAN TEKS SATUAN LITER ATAU RUPIAH
              Text(unit, style: const TextStyle(fontSize: 12, color: Colors.grey)),
            ],
          ),
        ],
      ),
    );
  }
}