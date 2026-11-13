import sys
import time
import pygame
from datetime import datetime
import Modules.varGlobals as varGlobals

from Modules.colors import (
    ttsMid1,
    ttsMid2,
    ttsMidLeft1,
    ttsMidLeft2,
    custom as cc
)

from Modules.varGlobals import (
    allLayout as layButton,
    login_page as lpButton,
    main_menu_page as mmButton
)

from Modules.getData import (
    cekLoginAdmin,
    getNamaPeternak,
    getProfilPeternak,
    getHargaHarianLokal,
    getRiwayatSetoranTPS,
    getDashboardTpsHariIni,
    updateHargaHarianLokal,
    getRiwayatWaktuDanVolume,
    getRiwayatSetoranPeternak
)

from Modules.logika import (
    layoutData,
    fillText_Login,
    fillText_MainMenu,
    layoutPencarianNama,
)

def loginPage():
    click = False

    varGlobals.runLogin    = True
    varGlobals.runMainMenu = False

    buttons = {
        "USERNAME" : lpButton.USERNAME,
        "PASSWORD" : lpButton.PASSWORD
    }

    done = {
        "LOGIN"    : lpButton.LOGIN
    }

    if not hasattr(varGlobals, 'username') or varGlobals.username is None: varGlobals.username = ""
    if not hasattr(varGlobals, 'password') or varGlobals.password is None: varGlobals.password = ""

    pesanError = ""
    startTime  = 0
    duration   = 3000

    while varGlobals.runLogin:
        varGlobals.screen.blit(varGlobals.bgLogin, (0, 0))

        mx, my = pygame.mouse.get_pos()
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                varGlobals.runLogin = False
                pygame.quit()
                sys.exit()
            if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                click = True
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN or event.key == pygame.K_KP_ENTER:
                    valid = cekLoginAdmin(varGlobals.username, varGlobals.password)
                    if valid:
                        varGlobals.runLogin = False
                        mainMenuPage()
                    elif varGlobals.username == "" or varGlobals.password == "":
                        pesanError = "USERNAME atau PASSWORD masih kosong!"
                        startTime  = pygame.time.get_ticks()
                    else:
                        print("Login Gagal!")
                        varGlobals.password = ""
                        pesanError = "USERNAME atau PASSWORD anda salah!"
                        startTime = pygame.time.get_ticks()

        for button, rect in buttons.items():
            current_values = {
                "USERNAME": varGlobals.username,
                "PASSWORD": varGlobals.password
            }
            
            val = current_values[button]
            if val == "":
                tampilanTeks = button
            else:
                tampilanTeks = "*" * len(val) if button == "PASSWORD" else val

            if rect.collidepoint(mx, my):
                pygame.draw.rect(varGlobals.screen, cc.WHITE, rect, border_radius=30)
                ttsMidLeft1(tampilanTeks, cc.GREY, rect, varGlobals.screen, 25)
                if click:
                    new_text = fillText_Login(button, buttons, done, current_values)
                    
                    if button == "USERNAME":
                        varGlobals.username = new_text
                    elif button == "PASSWORD":
                        varGlobals.password = new_text
            else:
                pygame.draw.rect(varGlobals.screen, cc.WHITE, rect, border_radius=20)
                ttsMid1(tampilanTeks, cc.GREY, rect, varGlobals.screen, 25)

        for button, rect in done.items():
            if rect.collidepoint(mx, my):
                pygame.draw.rect(varGlobals.screen, cc.THEME_E5, rect, border_radius=30)
                ttsMid1(button, cc.WHITE, rect, varGlobals.screen, 30)
                if click:
                    valid = cekLoginAdmin(varGlobals.username, varGlobals.password)
                    if valid:
                        varGlobals.runLogin = False
                        mainMenuPage()
                    elif varGlobals.username == "" or varGlobals.password == "":
                        pesanError = "USERNAME atau PASSWORD masih kosong!"
                        startTime  = pygame.time.get_ticks()
                    else:
                        varGlobals.password = ""
                        pesanError          = "USERNAME atau PASSWORD anda salah!"
                        startTime           = pygame.time.get_ticks()
            else:
                pygame.draw.rect(varGlobals.screen, cc.THEME_E4, rect, border_radius=20)
                ttsMid1(button, cc.WHITE, rect, varGlobals.screen, 25)

        if pesanError != "":
            waktu_sekarang = pygame.time.get_ticks()
            if waktu_sekarang - startTime < duration:
                error_rect = pygame.Rect(704, 1010, 1216, 70)
                pygame.draw.rect(varGlobals.screen, cc.RED_BROWN, error_rect, border_radius=0)
                ttsMidLeft1(pesanError, cc.WHITE, error_rect, varGlobals.screen, 25, padding=15)
            else:
                pesanError = ""

        click = False
        varGlobals.clock.tick(20)
        pygame.display.flip()

def mainMenuPage():
    click = False
    varGlobals.runLogin    = False
    varGlobals.runMainMenu = True

    buttons = {
        "TPS 1" : mmButton.TPS_1,
        "TPS 2" : mmButton.TPS_2,
        "EXIT"  : mmButton.EXIT
    }

    tps1           = False
    tps2           = False
    peternak       = False
    hasilPencarian = []

    searchBox = {
        "Cari Anggota Peternak" : mmButton.SEARCH
    }

    done = {
        varGlobals.pencarian : mmButton.CARI
    }

    rect_input_harga = pygame.Rect(50, 550, 229, 65)
    rect_btn_harga   = pygame.Rect(285, 550, 150, 65)
    
    priceBox = {"INPUT_HARGA" : rect_input_harga}
    priceBtn = {"Simpan" : rect_btn_harga}

    if not hasattr(varGlobals, 'inputHarga'): 
        varGlobals.inputHarga = str(getHargaHarianLokal())

    if not hasattr(varGlobals, 'search') or varGlobals.search is None: varGlobals.search = ""

    waktu_sekarang = datetime.now()

    while varGlobals.runMainMenu:
        varGlobals.screen.blit(varGlobals.bgMenu, (0, 0))

        aksi_ketik = None 
        key_target = None
        val_target = None

        mx, my = pygame.mouse.get_pos()
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                varGlobals.runMainMenu = False
                pygame.quit()
                sys.exit()

            if event.type == pygame.MOUSEWHEEL:
                if layButton.RIWAYAT.collidepoint(mx, my):
                    if not hasattr(varGlobals, 'scroll_y'):
                        varGlobals.scroll_y = 0
                
                    varGlobals.scroll_y -= event.y * 30
                
            if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                if tps1 or tps2 or peternak:
                    areaDashboard = [
                        layButton.GRAFIK, 
                        layButton.VOLUME, 
                        layButton.ID_TPS, 
                        layButton.RIWAYAT, 
                        layButton.KREDIT,
                        mmButton.SEARCH,
                        mmButton.CARI,
                        rect_input_harga,
                        rect_btn_harga
                    ]
                    
                    if hasattr(varGlobals, 'dapatNama') and varGlobals.dapatNama != "":
                        jml_data = len(hasilPencarian)
                        tinggi_drop = (jml_data * 45) if jml_data > 0 else 45
                        kotak_pencarian = list(searchBox.values())[0]
                        rect_dropdown = pygame.Rect(60, kotak_pencarian.bottom + 10, 400, tinggi_drop)
                        areaDashboard.append(rect_dropdown)

                    kenaKotak = any(shape.collidepoint(event.pos) for shape in areaDashboard)
                    if not kenaKotak:
                        tps1     = False
                        tps2     = False
                        peternak = False
                        varGlobals.search = ""
                    else:
                        click = True    
                else:
                    click = True

        pygame.draw.aaline(varGlobals.screen, cc.THEME_C3, (60, 175), (425, 175))
        pygame.draw.aaline(varGlobals.screen, cc.THEME_C3, (60, 325), (425, 325))

        for button, rect in buttons.items():
            if rect.collidepoint(mx, my):
                pygame.draw.rect(varGlobals.screen, cc.THEME_C2, rect, border_radius=20)
                ttsMidLeft2(button, cc.THEME_C6, rect, varGlobals.screen, 32)
                
                if click:
                    if button == "TPS 1":
                        tps1 = True
                        tps2 = False
                        peternak = False
                        
                        data_tps = getRiwayatSetoranTPS("1") 
                        vol_keseluruhan = 0.0
                        vol_bulan_ini = 0.0
                        pend_bulan_ini = 0.0
                        
                        for b in data_tps:
                            liter = float(b['volume_liter'])
                            pend_db = b.get('total_pendapatan')
                            pend_baris = float(pend_db) if pend_db is not None else (liter * 7500)
                            
                            vol_keseluruhan += liter
                            
                            tgl_str = str(b.get('waktu_setor', ''))
                            if tgl_str:
                                try:
                                    tgl_obj = datetime.fromisoformat(tgl_str.replace('Z', '+00:00'))
                                    if tgl_obj.year == waktu_sekarang.year and tgl_obj.month == waktu_sekarang.month:
                                        vol_bulan_ini += liter
                                        pend_bulan_ini += pend_baris
                                except Exception as e_waktu:
                                    print(f"Lewati baris karena format waktu gagal diparse: {e_waktu}")
                            
                        varGlobals.totalVolumeKeseluruhan = vol_keseluruhan
                        varGlobals.totalVolumeBulanIni = vol_bulan_ini
                        varGlobals.totalPendapatanBulanIni = pend_bulan_ini

                        varGlobals.dataRiwayat = data_tps[:15]
                        if len(data_tps) > 0:
                            varGlobals.dataVolume = [float(b['volume_liter']) for b in reversed(data_tps[:7])]
                        else:
                            varGlobals.dataVolume = []
                            
                        varGlobals.profilPeternak = {"nama_peternak": "Gabungan Semua Peternak", "id_kios": "TPS : 1"}
                        
                    elif button == "TPS 2":
                        tps1 = False
                        tps2 = True
                        peternak = False 
                        
                        data_tps = getRiwayatSetoranTPS("2") 
                        vol_keseluruhan = 0.0
                        vol_bulan_ini = 0.0
                        pend_bulan_ini = 0.0
                        
                        for b in data_tps:
                            liter = float(b['volume_liter'])
                            pend_db = b.get('total_pendapatan')
                            pend_baris = float(pend_db) if pend_db is not None else (liter * 7500)
                            
                            vol_keseluruhan += liter
                            
                            tgl_str = str(b.get('waktu_setor', ''))
                            if tgl_str:
                                try:
                                    tgl_obj = datetime.fromisoformat(tgl_str.replace('Z', '+00:00'))
                                    if tgl_obj.year == waktu_sekarang.year and tgl_obj.month == waktu_sekarang.month:
                                        vol_bulan_ini += liter
                                        pend_bulan_ini += pend_baris
                                except Exception as e_waktu:
                                    print(f"Lewati baris karena format waktu gagal diparse: {e_waktu}")
                            
                        varGlobals.totalVolumeKeseluruhan = vol_keseluruhan
                        varGlobals.totalVolumeBulanIni = vol_bulan_ini
                        varGlobals.totalPendapatanBulanIni = pend_bulan_ini

                        varGlobals.dataRiwayat = data_tps[:15]
                        if len(data_tps) > 0:
                            varGlobals.dataVolume = [float(b['volume_liter']) for b in reversed(data_tps[:7])]
                        else:
                            varGlobals.dataVolume = []
                            
                        varGlobals.profilPeternak = {"nama_peternak": "Gabungan Semua Peternak", "id_kios": "TPS : 2"}

                    elif button == "EXIT":
                        varGlobals.runMainMenu = False
                        pygame.quit()
                        sys.exit()
            else:
                pygame.draw.rect(varGlobals.screen, cc.THEME_C1, rect, border_radius=20)
                ttsMidLeft2(button, cc.THEME_C6, rect, varGlobals.screen, 30)

        for key, rect in searchBox.items():
            current_values = {key : varGlobals.search}
            val = current_values[key]
            tampilanTeks = val if val != "" else key

            if rect.collidepoint(mx, my):
                pygame.draw.rect(varGlobals.screen, cc.WHITE, rect, border_radius=20)
                pygame.draw.rect(varGlobals.screen, cc.THEME_C6, rect, 3, border_radius=20)
                ttsMidLeft1(tampilanTeks, cc.GREY, rect, varGlobals.screen, 25)
                if click:
                    aksi_ketik = "CARI"
                    key_target = key
                    val_target = current_values
            else:
                pygame.draw.rect(varGlobals.screen, cc.WHITE, rect, border_radius=20)
                pygame.draw.rect(varGlobals.screen, cc.THEME_C6, rect, 3, border_radius=20)
                ttsMidLeft1(tampilanTeks, cc.GREY, rect, varGlobals.screen, 23)

        for key, rect in done.items():
            if rect.collidepoint(mx, my):
                pygame.draw.rect(varGlobals.screen, cc.THEME_C5, rect, border_radius=20)
                if key:
                    if click:
                        logoSearch = pygame.transform.scale(key, (25, 25))
                        icon_rect  = logoSearch.get_rect(center=rect.center)
                        varGlobals.screen.blit(logoSearch, icon_rect)
                        
                        varGlobals.dapatNama = getNamaPeternak(varGlobals.search)
                        hasilPencarian       = varGlobals.dapatNama
                    else:
                        logoSearch = pygame.transform.scale(key, (30, 30))
                        icon_rect  = logoSearch.get_rect(center=rect.center)
                        varGlobals.screen.blit(logoSearch, icon_rect)
            else:
                pygame.draw.rect(varGlobals.screen, cc.THEME_C6, rect, border_radius=20)
                if key:
                    logoSearch = pygame.transform.scale(key, (25, 25))
                    icon_rect  = logoSearch.get_rect(center=rect.center)
                    varGlobals.screen.blit(logoSearch, icon_rect)

        for key, rect in priceBox.items():
            val = varGlobals.inputHarga
            tampilanTeks = f"Rp {val}" if val != "" else "Set Harga..."

            if rect.collidepoint(mx, my):
                pygame.draw.rect(varGlobals.screen, cc.WHITE, rect, border_radius=20)
                pygame.draw.rect(varGlobals.screen, cc.THEME_C6, rect, 3, border_radius=20)
                ttsMidLeft1(tampilanTeks, cc.GREY, rect, varGlobals.screen, 25)
                if click:
                    aksi_ketik = "HARGA"
                    key_target = key
                    val_target = {key: varGlobals.inputHarga}
            else:
                pygame.draw.rect(varGlobals.screen, cc.WHITE, rect, border_radius=20)
                pygame.draw.rect(varGlobals.screen, cc.THEME_C6, rect, 3, border_radius=20)
                ttsMidLeft1(tampilanTeks, cc.GREY, rect, varGlobals.screen, 23)
        
        for key, rect in priceBtn.items():
            if rect.collidepoint(mx, my):
                pygame.draw.rect(varGlobals.screen, cc.THEME_C5, rect, border_radius=20)
                ttsMid1(key, cc.WHITE, rect, varGlobals.screen, 25)
                if click and varGlobals.inputHarga != "":
                    sukses = updateHargaHarianLokal(int(varGlobals.inputHarga))
                    if sukses:
                        print(f"Berhasil mengubah harga harian menjadi: Rp {varGlobals.inputHarga}")
                    tps1, tps2 = False, False
            else:
                pygame.draw.rect(varGlobals.screen, cc.THEME_C6, rect, border_radius=20)
                ttsMid1(key, cc.WHITE, rect, varGlobals.screen, 23)

        # RENDER DASHBOARD DULUAN
        if tps1 or tps2 or peternak:
            layoutData()

        # RENDER DROPDOWN DI ATAS DASHBOARD
        if hasattr(varGlobals, 'dapatNama') and varGlobals.dapatNama != "":
            hasilNama, dropdown, hapusKlik = layoutPencarianNama(
                hasilPencarian, mx, my, click
            )

            if hasilNama:
                peternak          = True
                varGlobals.search = hasilNama
                
                if hasattr(varGlobals, 'totalVolumeKeseluruhan'):
                    del varGlobals.totalVolumeKeseluruhan
                
                data_db = getRiwayatSetoranPeternak(hasilNama)
                if data_db and len(data_db) > 0:
                    try:
                        waktu_sekarang = datetime.now()
                        vol_bulan_ini = 0.0
                        vol_keseluruhan = 0.0
                        pend_bulan_ini = 0.0

                        for b in data_db:
                            liter = float(b.get('volume_liter', 0))
                            pend_db = b.get('total_pendapatan')
                            # Mengatasi data lama yang masih NULL
                            pend_baris = float(pend_db) if pend_db is not None else (liter * 7500)
                            
                            vol_keseluruhan += liter

                            tgl_str = str(b.get('waktu_setor', '')) 
                            
                            if tgl_str:
                                try:
                                    tgl_obj = datetime.fromisoformat(tgl_str.replace('Z', '+00:00'))
                                    
                                    # Hanya hitung bulan ini
                                    if tgl_obj.year == waktu_sekarang.year and tgl_obj.month == waktu_sekarang.month:
                                        vol_bulan_ini += liter
                                        pend_bulan_ini += pend_baris
                                except Exception as e_waktu:
                                    print(f"Lewati baris karena format waktu gagal diparse: {e_waktu}")
                        
                        varGlobals.totalVolumeKeseluruhan = vol_keseluruhan
                        varGlobals.totalVolumeBulanIni = vol_bulan_ini
                        varGlobals.totalPendapatanBulanIni = pend_bulan_ini
                    except Exception as e:
                        varGlobals.totalVolumeKeseluruhan = 0.0
                        varGlobals.totalVolumeBulanIni = 0.0
                        print(f"Error parse volume: {e}")

                    # BARU POTONG DATANYA UNTUK KEPERLUAN UI
                    varGlobals.dataRiwayat = data_db[:15]
                    varGlobals.dataVolume = [float(b['volume_liter']) for b in reversed(data_db[:7])]
                    varGlobals.profilPeternak = {
                        "nama_peternak": data_db[0].get('nama_peternak', hasilNama),
                        "id_kios": data_db[0].get('id_kios', '-'),
                        "alamat": data_db[0].get('alamat', '-')
                    }
                else:
                    varGlobals.totalVolumeKeseluruhan = 0.0
                    varGlobals.dataRiwayat = []
                    varGlobals.dataVolume = []
                    varGlobals.profilPeternak = {}
                
            if dropdown:
                varGlobals.dapatNama = ""
                
            if hapusKlik:
                click = False

        if aksi_ketik == "CARI":
            varGlobals.search = fillText_MainMenu(key_target, searchBox, done, val_target)
            tps1 = False
            tps2 = False
        elif aksi_ketik == "HARGA":
            hasil_ketik = fillText_MainMenu(key_target, priceBox, priceBtn, val_target)
            varGlobals.inputHarga = ''.join(filter(str.isdigit, hasil_ketik))
            tps1 = False
            tps2 = False

        click = False
        varGlobals.clock.tick(20)
        pygame.display.flip()

mainMenuPage()
