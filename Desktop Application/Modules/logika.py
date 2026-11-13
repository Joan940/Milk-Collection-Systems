import sys
import pygame
import Modules.varGlobals as varGlobals

from Modules.colors import (
    ttsMid1,
    ttsMid2,
    ttsTopLeft1,
    ttsMidLeft1,
    ttsMidLeft2,
    ttsMid3,
    ttsMidLeft3,
    ttsTopLeft3,
    ttsMid4,
    ttsMidLeft4,
    ttsTopLeft4,
    custom as cc
)

from Modules.varGlobals import (
    allLayout as layButton
)

def fillText_Login(inp_key, inputUser_rects, done_rect, current_values):
    bg_snapshot  = varGlobals.screen.copy()
    current_text = current_values.get(inp_key, "")

    while True:
        varGlobals.screen.blit(bg_snapshot, (0, 0))
    
        for itemKu, rect in done_rect.items():
            pygame.draw.rect(varGlobals.screen, cc.THEME_E4, rect, border_radius=20)
            ttsMid1(itemKu, cc.WHITE, rect, varGlobals.screen, 25)
    
        for key, rect in inputUser_rects.items():
            val = current_text if key == inp_key else current_values.get(key, "")
            
            if val == "":
                tampilan_teks = key
            else:
                tampilan_teks = "*" * len(val) if key == "PASSWORD" else val
                
            if key == inp_key:
                pygame.draw.rect(varGlobals.screen, cc.WHITE, rect, border_radius=30)
                ttsMidLeft1(tampilan_teks, cc.BLACK, rect, varGlobals.screen, 25)
            else:
                pygame.draw.rect(varGlobals.screen, cc.WHITE, rect, border_radius=20)
                ttsMid1(tampilan_teks, cc.GREY, rect, varGlobals.screen, 25)

        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_BACKSPACE:
                    current_text = current_text[:-1]
                elif event.key == pygame.K_RETURN or event.key == pygame.K_KP_ENTER:
                    pygame.event.post(event)
                    return current_text 
                else:
                    if event.unicode.isprintable():
                        current_text += event.unicode
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if not inputUser_rects[inp_key].collidepoint(event.pos):
                    pygame.event.post(event)
                    return current_text 

        varGlobals.clock.tick(30)
        pygame.display.flip()

def fillText_MainMenu(inp_key, inputUser_rects, done_rect, current_values):
    bg_snapshot  = varGlobals.screen.copy()
    current_text = current_values.get(inp_key, "")

    while True:
        varGlobals.screen.blit(bg_snapshot, (0, 0))
    
        for key_icon, rect in done_rect.items():
            pygame.draw.rect(varGlobals.screen, cc.THEME_C6, rect, border_radius=20)
            if key_icon:
                if isinstance(key_icon, pygame.Surface):
                    logoSearch = pygame.transform.scale(key_icon, (25, 25))
                    icon_rect  = logoSearch.get_rect(center=rect.center)
                    varGlobals.screen.blit(logoSearch, icon_rect)
                elif isinstance(key_icon, str):
                    ttsMid1(key_icon, cc.WHITE, rect, varGlobals.screen, 23)
    
        for key, rect in inputUser_rects.items():
            val = current_text if key == inp_key else current_values.get(key, "")
            
            if val == "":
                tampilan_teks = key
            else:
                tampilan_teks = val
                
            pygame.draw.rect(varGlobals.screen, cc.WHITE, rect, border_radius=20)
            if key == inp_key:
                pygame.draw.rect(varGlobals.screen, cc.THEME_C6, rect, 3, border_radius=20)
            else:
                pygame.draw.rect(varGlobals.screen, cc.THEME_C6, rect, 3, border_radius=20)
            ttsMidLeft1(tampilan_teks, cc.BLACK, rect, varGlobals.screen, 25)

        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_BACKSPACE:
                    current_text = current_text[:-1]
                elif event.key == pygame.K_RETURN or event.key == pygame.K_KP_ENTER:
                    pygame.event.post(event)
                    return current_text 
                else:
                    if event.unicode.isprintable():
                        current_text += event.unicode
            elif event.type == pygame.MOUSEBUTTONDOWN:
                if not inputUser_rects[inp_key].collidepoint(event.pos):
                    pygame.event.post(event)
                    return current_text 

        varGlobals.clock.tick(30)
        pygame.display.flip()

def layoutData():
    pygame.draw.rect(varGlobals.screen, cc.WHITE, layButton.GRAFIK, border_radius=20)
    pygame.draw.rect(varGlobals.screen, cc.WHITE, layButton.ID_TPS, border_radius=20)
    pygame.draw.rect(varGlobals.screen, cc.WHITE, layButton.VOLUME, border_radius=20)
    pygame.draw.rect(varGlobals.screen, cc.WHITE, layButton.RIWAYAT, border_radius=20)
    pygame.draw.rect(varGlobals.screen, cc.WHITE, layButton.KREDIT, border_radius=20)

    # 1. GRAFIK & HITUNG TOTAL VOLUME
    if hasattr(varGlobals, 'totalVolumeBulanIni'):
        total_vol_bulan_ini = varGlobals.totalVolumeBulanIni
        total_vol_keseluruhan = varGlobals.totalVolumeKeseluruhan 
    elif hasattr(varGlobals, 'dataVolume') and len(varGlobals.dataVolume) > 0:
        total_vol_bulan_ini = sum(varGlobals.dataVolume)
        total_vol_keseluruhan = sum(varGlobals.dataVolume)
    else:
        total_vol_bulan_ini = 0.0
        total_vol_keseluruhan = 0.0

    if hasattr(varGlobals, 'dataVolume') and len(varGlobals.dataVolume) > 0:
        gambarGrafikTransaksi(layButton.GRAFIK, varGlobals.dataVolume)
    else:
        rect_kosong_grafik = pygame.Rect(layButton.GRAFIK.x + 30, layButton.GRAFIK.y + 30, 400, 40)
        ttsMidLeft4("Belum ada data setoran", cc.THEME_C4, rect_kosong_grafik, varGlobals.screen, 25)

    # 2. TOTAL VOLUME
    rect_judul_vol = pygame.Rect(layButton.ID_TPS.x + 20, layButton.ID_TPS.y + 15, 250, 50)
    ttsMid4("Vol. Bulan Ini", cc.THEME_C4, rect_judul_vol, varGlobals.screen, 30)
    ttsMid3(f"{total_vol_bulan_ini:.1f}", cc.THEME_C6, layButton.ID_TPS, varGlobals.screen, 60)

    # 3. TOTAL PENDAPATAN
    if hasattr(varGlobals, 'totalPendapatanBulanIni'):
        pendapatan = varGlobals.totalPendapatanBulanIni
    elif hasattr(varGlobals, 'dataVolume') and len(varGlobals.dataVolume) > 0:
        total_vol_bulan_ini = sum(varGlobals.dataVolume)
        pendapatan = total_vol_bulan_ini * 7500
    else:
        pendapatan = 0.0

    format_rp = f"{pendapatan:,.2f}".replace(",", "X").replace(".", ",").replace("X", ".")
    rect_judul_rp = pygame.Rect(layButton.VOLUME.x + 20, layButton.VOLUME.y + 15, 200, 50)
    ttsMid4("Pendapatan Bulan Ini", cc.THEME_C4, rect_judul_rp, varGlobals.screen, 30)
    ttsMid4(f"Rp {format_rp}", cc.THEME_C6, layButton.VOLUME, varGlobals.screen, 60)

    # 4. PROFIL
    nama_tampil = varGlobals.profilPeternak.get('nama_peternak', '-') if hasattr(varGlobals, 'profilPeternak') and varGlobals.profilPeternak else '-'
    id_tps = varGlobals.profilPeternak.get('id_kios', '-') if hasattr(varGlobals, 'profilPeternak') and varGlobals.profilPeternak else '-' 

    rect_judul_kredit = pygame.Rect(layButton.KREDIT.x + 15, layButton.KREDIT.y + 15, 200, 50)
    rect_nama         = pygame.Rect(layButton.KREDIT.x + 15, layButton.KREDIT.y + 70, 300, 35)
    rect_id           = pygame.Rect(layButton.KREDIT.x + 15, layButton.KREDIT.y + 100, 300, 25)

    ttsMidLeft4("Informasi Data", cc.THEME_C4, rect_judul_kredit, varGlobals.screen, 30)
    ttsMidLeft4(nama_tampil, cc.THEME_C6, rect_nama, varGlobals.screen, 30)
    ttsMidLeft4(f"{id_tps}", cc.THEME_C6, rect_id, varGlobals.screen, 20)

    rect_judul_riwayat = pygame.Rect(layButton.RIWAYAT.x + 20, layButton.RIWAYAT.y + 15, 300, 50)
    ttsMidLeft4("Riwayat Penyetoran", cc.THEME_C4, rect_judul_riwayat, varGlobals.screen, 30)

    if hasattr(varGlobals, 'dataRiwayat') and len(varGlobals.dataRiwayat) > 0:
        start_y = layButton.RIWAYAT.y + 80
        tinggi_baris = 70
        
        if not hasattr(varGlobals, 'scroll_y'):
            varGlobals.scroll_y = 0

        visible_height = layButton.RIWAYAT.height - 90
        total_content_height = len(varGlobals.dataRiwayat) * tinggi_baris
        
        max_scroll = max(0, total_content_height - visible_height)
        varGlobals.scroll_y = max(0, min(varGlobals.scroll_y, max_scroll))

        clip_rect = pygame.Rect(layButton.RIWAYAT.x, start_y, layButton.RIWAYAT.width, visible_height)
        varGlobals.screen.set_clip(clip_rect)

        for i, data in enumerate(varGlobals.dataRiwayat):
            if i >= 15: break

            baris_y = start_y + (i * tinggi_baris) - varGlobals.scroll_y
            
            vol_setor = data['volume_liter']
            waktu_obj = data['waktu_setor']
            
            try:
                tanggal = waktu_obj.strftime("%d %b %Y")
                jam = waktu_obj.strftime("%H:%M")
            except:
                tanggal = str(waktu_obj)[:50]
                jam = str(waktu_obj)[11:16]

            if hasattr(varGlobals, 'totalVolumeKeseluruhan'):
                nama_riwayat = data.get('nama_peternak', '-')
                rect_nama = pygame.Rect(layButton.RIWAYAT.x + 30, baris_y, 250, 25)
                rect_waktu = pygame.Rect(layButton.RIWAYAT.x + 30, baris_y + 25, 250, 20)
                ttsMidLeft4(nama_riwayat, cc.THEME_C6, rect_nama, varGlobals.screen, 22)
                ttsMidLeft4(f"{tanggal} - {jam}", cc.THEME_C4, rect_waktu, varGlobals.screen, 18)
            else:
                rect_waktu = pygame.Rect(layButton.RIWAYAT.x + 30, baris_y + 10, 250, 30)
                ttsMidLeft4(f"{tanggal} - {jam}", cc.THEME_C6, rect_waktu, varGlobals.screen, 24)

            rect_vol = pygame.Rect(layButton.RIWAYAT.right - 180, baris_y + 10, 150, 30)
            ttsMidLeft4(f"{vol_setor} Ltr", cc.THEME_C6, rect_vol, varGlobals.screen, 24)

            if i < len(varGlobals.dataRiwayat) - 1:
                pygame.draw.line(varGlobals.screen, (240, 240, 240), (layButton.RIWAYAT.x + 30, baris_y + 60), (layButton.RIWAYAT.right - 30, baris_y + 60), 2)
        
        varGlobals.screen.set_clip(None)
    else:
        rect_kosong = pygame.Rect(layButton.RIWAYAT.x + 30, layButton.RIWAYAT.y + 80, 300, 30)
        ttsMidLeft4("Belum ada riwayat", cc.THEME_C4, rect_kosong, varGlobals.screen, 22)

def layoutPencarianNama(hasilPencarian, mx, my, click):
    dropX = varGlobals.window_rect.centerx - (varGlobals.PANJANG_BUTTON * 4.73)
    dropW = varGlobals.PANJANG_BUTTON * 2
    dropY = varGlobals.window_rect.centery - (varGlobals.LEBAR_BUTTON * 0.4)
    dropH = 45

    dropdown   = False
    hapusKlik  = False
    daftarNama = None

    if len(hasilPencarian) > 0:
        totalTinggi = len(hasilPencarian) * dropH + 6
        bgDropdown  = pygame.Rect(dropX, dropY, dropW, totalTinggi)

        pygame.draw.rect(varGlobals.screen, cc.WHITE, bgDropdown, border_radius=20)
        pygame.draw.rect(varGlobals.screen, cc.BLACK, bgDropdown, 3, border_radius=20)

        for i, nama in enumerate(hasilPencarian):
            baris = pygame.Rect(dropX + 5, dropY + (i * dropH), dropW, dropH)

            if baris.collidepoint(mx, my):
                hover = pygame.Rect(dropX + 5, dropY + (i * dropH) + 5, dropW - 10, dropH - 4)
                pygame.draw.rect(varGlobals.screen, cc.THEME_C2, hover, border_radius=16)
                if click:
                    daftarNama = nama
                    dropdown   = True
                    hapusKlik  = True

            kotakTeks = pygame.Rect(dropX + 20, dropY + (i * dropH), dropW - 20, dropH)
            ttsMidLeft1(nama, cc.BLACK, kotakTeks, varGlobals.screen, 22)
    else:
        bgDropdown = pygame.Rect(dropX, dropY, dropW, dropH)
        pygame.draw.rect(varGlobals.screen, cc.WHITE, bgDropdown, border_radius=15)
        pygame.draw.rect(varGlobals.screen, cc.THEME_C6, bgDropdown, 3, border_radius=15)

        kotakTeks = pygame.Rect(dropX + 20, dropY, dropW - 20, dropH)
        ttsMidLeft1("Data tidak ditemukan", cc.GREY, kotakTeks, varGlobals.screen, 22)
        if click:
            dropdown  = True
            hapusKlik = True

    return daftarNama, dropdown, hapusKlik

def gambarGrafikTransaksi(rect_grafik, data_volume):
    if not data_volume or len(data_volume) < 2:
        ttsMidLeft4("Data setoran belum cukup untuk grafik", cc.GREY, rect_grafik, varGlobals.screen, 25)
        return

    margin_x_kiri = 80
    margin_x_kanan = 40
    margin_y = 60

    # 2. Area Efektif 
    area_w = rect_grafik.width - margin_x_kiri - margin_x_kanan
    area_h = rect_grafik.height - (2 * margin_y)

    start_x = rect_grafik.left + margin_x_kiri
    bottom_y = rect_grafik.bottom - margin_y 

    max_vol = max(data_volume)
    if max_vol == 0: 
        max_vol = 1 
        
    skala_maks = max_vol + (max_vol * 0.2) 

    jumlah_garis_bantu = 5     
    for i in range(jumlah_garis_bantu):
        nilai_label = (skala_maks / (jumlah_garis_bantu - 1)) * i
        y_garis = bottom_y - ((nilai_label / skala_maks) * area_h)
        
        teks_rect = pygame.Rect(rect_grafik.left + 10, y_garis - 10, 60, 20)
        ttsMidLeft4(f"{nilai_label:.1f}", cc.GREY, teks_rect, varGlobals.screen, 18)

        for x_garis in range(int(start_x), int(start_x + area_w), 15):
            pygame.draw.line(
                varGlobals.screen, 
                (220, 220, 220),
                (x_garis, y_garis), 
                (x_garis + 8, y_garis), 
                1
            )

    jml_data = len(data_volume)
    step_x = area_w / (jml_data - 1)

    points = []
    for i, vol in enumerate(data_volume):
        x = start_x + (i * step_x)
        y = bottom_y - ((vol / skala_maks) * area_h)
        points.append((x, y))

    pygame.draw.lines(varGlobals.screen, cc.THEME_C6, False, points, 4)

    for pt in points:
        pygame.draw.circle(varGlobals.screen, cc.THEME_C6, pt, 7)
        pygame.draw.circle(varGlobals.screen, cc.WHITE, pt, 3)