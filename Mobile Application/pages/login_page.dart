import 'package:flutter/material.dart';
import 'package:flutter_svg/flutter_svg.dart';
import 'package:google_fonts/google_fonts.dart';
import 'package:supabase_flutter/supabase_flutter.dart';
import 'mainmenu_page.dart';

// KELAS LOGIN PAGE UNTUK HALAMAN MASUK PETERNAK
class LoginPage extends StatefulWidget {
  const LoginPage({super.key});

  @override
  State<LoginPage> createState() => _LoginPageState();
}

class _LoginPageState extends State<LoginPage> {
  // INISIALISASI VARIABEL DAN KONTROLER INPUT
  final SupabaseClient supabase = Supabase.instance.client;
  final TextEditingController _usernameController = TextEditingController();
  final TextEditingController _passwordController = TextEditingController();

  bool _isPasswordVisible = false;
  bool _isLoading = false;

  // MEMBERSIHKAN MEMORI SAAT HALAMAN DITUTUP AGAR APLIKASI TIDAK BERAT
  @override
  void dispose() {
    _usernameController.dispose();
    _passwordController.dispose();
    super.dispose();
  }

  // FUNGSI UNTUK MENANGANI PROSES LOGIN
  Future<void> _handleLogin() async {
    // MENUTUP KEYBOARD SAAT TOMBOL DITEKAN AGAR TIDAK MENUTUPI PESAN ERROR
    FocusScope.of(context).unfocus(); 

    final String inputName = _usernameController.text.trim();
    final String inputPassword = _passwordController.text.trim();

    // CEK INPUT KOSONG
    if (inputName.isEmpty || inputPassword.isEmpty) {
      _showSnackBar("Nama dan Password tidak boleh kosong");
      return;
    }

    setState(() => _isLoading = true);

    try {
      // MENCARI DATA PETERNAK DI DATABASE BERSERTA PASSWORDNYA
      final peternakData = await supabase
        .from('tb_peternak')
        .select()
        .eq('nama_peternak', inputName)
        .eq('password_peternak', inputPassword)
        .maybeSingle();

      if (peternakData != null) {
        // NAVIGASI KE HALAMAN UTAMA JIKA BERHASIL LOGIN
        _onLoginSuccess(
          role: 'user', 
          name: peternakData['nama_peternak'].toString(),
          idUser: peternakData['id_peternak'].toString(),
        );
      } else {
        _showSnackBar("Akun tidak ditemukan atau UID salah!");
      }
    } catch (e) {
      if (mounted) {
        // MENAMPILKAN PESAN ERROR JIKA KONEKSI ATAU DATABASE BERMASALAH
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text("Error Detail: $e"), 
            duration: const Duration(seconds: 10),
            backgroundColor: Colors.red,
          ),
        );
      }
    } finally {
      if (mounted) setState(() => _isLoading = false);
    }
  }

  void _onLoginSuccess({required String role, required String name, required String idUser}) {
    debugPrint("Login Berhasil sebagai $role: $name");
    if (mounted) {
      Navigator.pushReplacement(
        context,
        MaterialPageRoute(
          builder: (context) => MainmenuPage(
            namaUserLogin: name,
            idUserLogin: idUser,
          ),
        ),
      );
    }
  }

  void _showSnackBar(String message) {
    if (mounted) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text(message), backgroundColor: Colors.redAccent),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color(0xFFEFEFE9),
      body: Center(
        // MENGGUNAKAN SCROLL AGAR AMAN SAAT KEYBOARD MUNCUL
        child: SingleChildScrollView(
          padding: const EdgeInsets.symmetric(horizontal: 30.0),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              // MENAMPILKAN LOGO DAN JUDUL APLIKASI
              Container(
                height: 140,
                width: 140,
                decoration: const BoxDecoration(
                  color: Color(0xFF959D90),
                  shape: BoxShape.circle,
                ),
                child: Padding(
                  padding: const EdgeInsets.all(5),
                  child: SvgPicture.asset(
                    'assets/icons/LOGO.svg',
                    colorFilter: const ColorFilter.mode(Colors.black, BlendMode.srcIn),
                  ),
                ),
              ),

              const SizedBox(height: 10),
              Text(
                "SMARTSILO",
                style: GoogleFonts.geo(
                  fontSize: 48,
                  fontWeight: FontWeight.bold,
                  color: const Color(0xFF523D35),
                ),
              ),
              const Text(
                "Kopeng Krajan",
                style: TextStyle(
                  color: Color(0xFF523D35),
                  fontWeight: FontWeight.w600,
                  letterSpacing: 1.2,
                ),
              ),

              const SizedBox(height: 40),

              // INPUT UNTUK NAMA PETERNAK
              TextField(
                controller: _usernameController,
                style: const TextStyle(color: Color(0xFF523D35)),
                decoration: InputDecoration(
                  labelText: "Nama",
                  prefixIcon: const Icon(Icons.person_outline, color: Color(0xFF523D35)),
                  filled: true,
                  fillColor: Colors.white.withValues(alpha: 0.5),
                  border: OutlineInputBorder(borderRadius: BorderRadius.circular(12)),
                ),
              ),

              const SizedBox(height: 15),

              // INPUT UNTUK PASSWORD DENGAN FITUR HIDE SHOW MATA
              TextField(
                controller: _passwordController,
                obscureText: !_isPasswordVisible,
                style: const TextStyle(color: Color(0xFF523D35)),
                decoration: InputDecoration(
                  labelText: "UID / Password",
                  prefixIcon: const Icon(Icons.lock_outline, color: Color(0xFF523D35)),
                  suffixIcon: IconButton(
                    icon: Icon(
                      _isPasswordVisible ? Icons.visibility : Icons.visibility_off,
                      color: const Color(0xFF523D35),
                    ),
                    onPressed: () => setState(() => _isPasswordVisible = !_isPasswordVisible),
                  ),
                  filled: true,
                  fillColor: Colors.white.withValues(alpha: 0.5),
                  border: OutlineInputBorder(borderRadius: BorderRadius.circular(12)),
                ),
              ),

              const SizedBox(height: 30),

              // TOMBOL MASUK DENGAN INDIKATOR LOADING
              SizedBox(
                width: double.infinity,
                height: 55,
                child: ElevatedButton(
                  style: ElevatedButton.styleFrom(
                    backgroundColor: const Color(0xFF523D35),
                    foregroundColor: Colors.white,
                    shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
                    elevation: 3,
                  ),
                  onPressed: _isLoading ? null : _handleLogin,
                  child: _isLoading
                      ? const CircularProgressIndicator(color: Colors.white)
                      : const Text(
                          "MASUK",
                          style: TextStyle(fontSize: 16, fontWeight: FontWeight.bold),
                        ),
                ),
              ),

              const SizedBox(height: 40),
              const Text(
                "Versi 1.0.0 - Tugas Talenta Unggul",
                style: TextStyle(color: Color(0xFF523D35), fontSize: 12),
              ),
            ],
          ),
        ),
      ),
    );
  }
}