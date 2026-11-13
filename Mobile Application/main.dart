import 'pages/login_page.dart';
import 'package:flutter/material.dart';
import 'package:supabase_flutter/supabase_flutter.dart';
import 'package:intl/date_symbol_data_local.dart';

final supabase = Supabase.instance.client;

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

  await initializeDateFormatting('id_ID', null);

  await Supabase.initialize(
    url: 'https://qrummydcfiftbkmgebkq.supabase.co',
    anonKey: 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InFydW1teWRjZmlmdGJrbWdlYmtxIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NzgwMjEyMTMsImV4cCI6MjA5MzU5NzIxM30.iBN2PGra-EM7NMDP5e1SpSW3XCR-Az0EeD3gUZHXm4Q',
  );

  runApp(const HalamanAwal());
}

class HalamanAwal extends StatelessWidget {
  const HalamanAwal({super.key});

  @override
  Widget build(BuildContext context) {
    return const MaterialApp(
      debugShowCheckedModeBanner: false,
      home: LoginPage(),
    );
  }
}
