import pygame
import os, sys

###################################################################################################
#                                           MAIN WINDOW                                           #
###################################################################################################

if sys.platform in ["win32", "win64"]: os.environ["SDL_VIDEO_CENTERED"] = "1"

pygame.init()
clock = pygame.time.Clock()

os.environ['SDL_VIDEO_CENTERED'] = '1'
info                             = pygame.display.Info()
screen_width, screen_height      = info.current_w, info.current_h
res                              = (screen_width, screen_height)

screen      = pygame.display.set_mode(res)
window_rect = pygame.Surface.get_rect(screen)
print(int(screen_width), int(screen_height))

###################################################################################################
#                                            VARIABLE                                             #
###################################################################################################

# NONE
currentBg = None

# STRING KOSONG
search    = ""
username  = ""
password  = ""
dapatNama = ""
alamatPeternak = ""

dataVolume     = []
profilPeternak = []

# BOOLEAN
runLogin    = bool
runMainMenu = bool

# MY MOON
NAMA_BULAN = {
    "01": "Januari", "02": "Februari", "03": "Maret", "04": "April",
    "05": "Mei", "06": "Juni", "07": "Juli", "08": "Agustus",
    "09": "September", "10": "Oktober", "11": "November", "12": "Desember"
}

# NOL
scroll_y = 0
totalVolumeBulanIni = 0
totalVolumeKeseluruhan = 0

# SETUP BUTTON
LEBAR_BUTTON        = res[1] * 0.06
PANJANG_BUTTON      = res[0] * 0.1

# BACKGROUND
bgLogin = pygame.image.load(r"D:\Joan\Kuliah_Boss\Kuliah\TTU\Alat\CODE_PC\Aplikasi_Desktop\Assets\loginPage.png")
bgMenu  = pygame.image.load(r"D:\Joan\Kuliah_Boss\Kuliah\TTU\Alat\CODE_PC\Aplikasi_Desktop\Assets\mainMenu.png")

# ICONS
pencarian = pygame.image.load(r"D:\Joan\Kuliah_Boss\Kuliah\TTU\Alat\CODE_PC\Aplikasi_Desktop\Assets\search.png")

# FONT PATH
F_norwester  = r"D:\Joan\Kuliah_Boss\Kuliah\TTU\Alat\CODE_PC\Aplikasi_Desktop\Assets\Norwester.otf"
F_geoReguler = r"D:\Joan\Kuliah_Boss\Kuliah\TTU\Alat\CODE_PC\Aplikasi_Desktop\Assets\Geo-Regular.ttf"
F_archivo    = r"D:\Joan\Kuliah_Boss\Kuliah\TTU\Alat\CODE_PC\Aplikasi_Desktop\Assets\ArchivoBlack-Regular.ttf"
F_neue       = r"D:\Joan\Kuliah_Boss\Kuliah\TTU\Alat\CODE_PC\Aplikasi_Desktop\Assets\BebasNeue-Regular.ttf"

# UKURAN & POSISI BUTTON 
class login_page:
    USERNAME = pygame.rect.Rect(
        window_rect.centerx - (PANJANG_BUTTON * 0.3),
        window_rect.centery - (LEBAR_BUTTON * 2.4),
        PANJANG_BUTTON * 4.25,
        LEBAR_BUTTON * 1.3
    )
    PASSWORD = pygame.rect.Rect(
        window_rect.centerx - (PANJANG_BUTTON * 0.3),
        window_rect.centery - (LEBAR_BUTTON * (-0.4)),
        PANJANG_BUTTON * 4.25,
        LEBAR_BUTTON * 1.3
    )
    LOGIN = pygame.rect.Rect(
        window_rect.centerx - (PANJANG_BUTTON * 0.3),
        window_rect.centery - (LEBAR_BUTTON * (-2)),
        PANJANG_BUTTON * 4.25,
        LEBAR_BUTTON * 1.3
    )

class main_menu_page:
    # NILAI Y DIKURANGI 1.1
    TPS_1 = pygame.rect.Rect(
        window_rect.centerx - (PANJANG_BUTTON * 4.73),
        window_rect.centery - (LEBAR_BUTTON * 5.5),
        PANJANG_BUTTON * 2,
        LEBAR_BUTTON * 1
    )
    TPS_2 = pygame.rect.Rect(
        window_rect.centerx - (PANJANG_BUTTON * 4.73),
        window_rect.centery - (LEBAR_BUTTON * 4.4),
        PANJANG_BUTTON * 2,
        LEBAR_BUTTON * 1
    )
    SEARCH = pygame.rect.Rect(
        window_rect.centerx - (PANJANG_BUTTON * 4.73),
        window_rect.centery - (LEBAR_BUTTON * 1.5),
        PANJANG_BUTTON * 1.6,
        LEBAR_BUTTON * 1
    )
    CARI = pygame.rect.Rect(
        window_rect.centerx - (PANJANG_BUTTON * 3.1),
        window_rect.centery - (LEBAR_BUTTON * 1.5),
        PANJANG_BUTTON * 0.37,
        LEBAR_BUTTON * 1
    )
    EXIT = pygame.rect.Rect(
        window_rect.centerx - (PANJANG_BUTTON * 4.73),
        window_rect.centery - (LEBAR_BUTTON * 3.19),
        PANJANG_BUTTON * 2,
        LEBAR_BUTTON * 1
    )

class allLayout:
    ID_TPS = pygame.rect.Rect(
        window_rect.centerx - (PANJANG_BUTTON * (-3.2)),
        window_rect.centery - (LEBAR_BUTTON * 7.55),
        PANJANG_BUTTON * 1.5,
        LEBAR_BUTTON * 5.4
    )
    GRAFIK = pygame.rect.Rect(
        window_rect.centerx - (PANJANG_BUTTON * 2.11),
        window_rect.centery - (LEBAR_BUTTON * 7.55),
        PANJANG_BUTTON * 4.98,
        LEBAR_BUTTON * 5.4
    )
    VOLUME = pygame.rect.Rect(
        window_rect.centerx - (PANJANG_BUTTON * 2.11),
        window_rect.centery - (LEBAR_BUTTON * 1.25),
        PANJANG_BUTTON * 2.5,
        LEBAR_BUTTON * 5.4
    )
    RIWAYAT = pygame.rect.Rect(
        window_rect.centerx - (PANJANG_BUTTON * (-0.74)),
        window_rect.centery - (LEBAR_BUTTON * 1.25),
        PANJANG_BUTTON * 3.95,
        LEBAR_BUTTON * 8.7
    )
    KREDIT = pygame.rect.Rect(
        window_rect.centerx - (PANJANG_BUTTON * 2.11),
        window_rect.centery - (LEBAR_BUTTON * (-5)),
        PANJANG_BUTTON * 2.5,
        LEBAR_BUTTON * 2.5
    )