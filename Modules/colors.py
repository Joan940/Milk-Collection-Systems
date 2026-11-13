import pygame
import Modules.varGlobals as varGlobals

class custom:
    RED             = (255, 0, 0)
    GREEN           = (0, 200, 0)
    BLUE            = (0, 0, 255)
    BLACK           = (0, 0, 0)
    WHITE           = (245, 245, 245)
    FTEK            = (209, 107, 2)
    FTEK2           = (150, 77, 2)
    DARK_BROWN      = (101, 67, 33)
    LIGHT_BROWN     = (181, 101, 29)
    RED_BROWN       = (165, 42, 42)
    YELLOW_BROWN    = (153, 101, 21)
    GRAY_BROWN      = (128, 105, 72)
    COFFEE_BROWN    = (111, 78, 55)
    COPPER_BROWN    = (184, 115, 51)
    LIGHT_RED_BROWN = (193, 154, 107)
    BRIGHT_YELLOW   = (255, 255, 0)
    ORANGE          = (255, 165, 0)
    PURPLE          = (128, 0, 128)
    CYAN            = (0, 255, 255)
    MAGENTA         = (255, 0, 255)
    GREY            = (192, 192, 192)
    DARK_GREY       = (64, 64, 64)
    LIGHT_BLUE      = (173, 216, 230)
    GOLD            = (255, 215, 0)
    SILVER          = (192, 192, 192)

    THEME_A1 = (211, 230, 243)  # #D3E6F3
    THEME_A2 = (146, 195, 227)  # #92C3E3
    THEME_A3 = (97, 158, 197)   # #619EC5
    THEME_A4 = (75, 124, 155)   # #4B7C9B
    THEME_A5 = (52, 88, 111)    # #34586F
    THEME_A6 = (29, 52, 67)     # #1D3443
    THEME_A7 = (8, 20, 28)      # #08141C

    # Row 2 (Muted Blue)
    THEME_B1 = (203, 223, 235)  # #CBDFEB
    THEME_B2 = (144, 188, 214)  # #90BCD6
    THEME_B3 = (108, 150, 175)  # #6C96AF
    THEME_B4 = (83, 117, 136)   # #537588
    THEME_B5 = (57, 81, 96)     # #395160
    THEME_B6 = (31, 46, 55)     # #1F2E37
    THEME_B7 = (8, 15, 19)      # #080F13

    # Row 3 (Steel Blue)
    THEME_C1 = (205, 223, 234)  # #CDDFEA
    THEME_C2 = (148, 188, 211)  # #94BCD3
    THEME_C3 = (112, 150, 172)  # #7096AC
    THEME_C4 = (86, 117, 134)   # #567586
    THEME_C5 = (59, 82, 94)     # #3B525E
    THEME_C6 = (32, 47, 55)     # #202F37
    THEME_C7 = (8, 15, 19)      # #080F13

    # Row 4 (Denim Blue)
    THEME_D1 = (250, 251, 253)  # #FAFBFD
    THEME_D2 = (198, 214, 228)  # #C6D6E4
    THEME_D3 = (146, 178, 204)  # #92B2CC
    THEME_D4 = (110, 144, 172)  # #6E90AC
    THEME_D5 = (82, 108, 129)   # #526C81
    THEME_D6 = (53, 71, 86)     # #354756
    THEME_D7 = (26, 37, 46)     # #1A252E

    # Row 5 (Slate Grey)
    THEME_E1 = (237, 239, 240)  # #EDEFF0
    THEME_E2 = (194, 201, 204)  # #C2C9CC
    THEME_E3 = (155, 163, 168)  # #9BA3A8
    THEME_E4 = (123, 130, 133)  # #7B8285
    THEME_E5 = (89, 95, 97)     # #595F61
    THEME_E6 = (56, 59, 61)     # #383B3D
    THEME_E7 = (25, 27, 28)     # #191B1C

def ttsMid1(text, color, rect, surface, font_size):
    font = pygame.font.Font(varGlobals.F_geoReguler, font_size)
    text_surface = font.render(str(text), True, color)
    text_rect = text_surface.get_rect(center=rect.center)
    surface.blit(text_surface, text_rect)

def ttsTopLeft1(text, color, rect, surface, font_size, padding=20):
    font = pygame.font.Font(varGlobals.F_geoReguler, font_size)
    text_surface = font.render(str(text), True, color)
    text_rect = text_surface.get_rect(left=(rect.left + padding))
    surface.blit(text_surface, text_rect)

def ttsMidLeft1(text, color, rect, surface, font_size, padding=20):
    font = pygame.font.Font(varGlobals.F_geoReguler, font_size)
    text_surface = font.render(text, True, color)
    text_rect = text_surface.get_rect(midleft=(rect.left + padding, rect.centery))
    surface.blit(text_surface, text_rect)

def ttsMid2(text, color, rect, surface, font_size):
    font = pygame.font.Font(varGlobals.F_norwester, font_size)
    text_surface = font.render(str(text), True, color)
    text_rect = text_surface.get_rect(center=rect.center)
    surface.blit(text_surface, text_rect)

def ttsMidLeft2(text, color, rect, surface, font_size, padding=20):
    font = pygame.font.Font(varGlobals.F_norwester, font_size)
    text_surface = font.render(text, True, color)
    text_rect = text_surface.get_rect(midleft=(rect.left + padding, rect.centery))
    surface.blit(text_surface, text_rect)

def ttsMid3(text, color, rect, surface, font_size):
    font = pygame.font.Font(varGlobals.F_archivo, font_size)
    text_surface = font.render(str(text), True, color)
    text_rect = text_surface.get_rect(center=rect.center)
    surface.blit(text_surface, text_rect)

def ttsTopLeft3(text, color, rect, surface, font_size, padding=20):
    font = pygame.font.Font(varGlobals.F_archivo, font_size)
    text_surface = font.render(str(text), True, color)
    text_rect = text_surface.get_rect(left=(rect.left + padding))
    surface.blit(text_surface, text_rect)

def ttsMidLeft3(text, color, rect, surface, font_size, padding=20):
    font = pygame.font.Font(varGlobals.F_archivo, font_size)
    text_surface = font.render(text, True, color)
    text_rect = text_surface.get_rect(midleft=(rect.left + padding, rect.centery))
    surface.blit(text_surface, text_rect)

def ttsMid4(text, color, rect, surface, font_size):
    font = pygame.font.Font(varGlobals.F_neue, font_size)
    text_surface = font.render(str(text), True, color)
    text_rect = text_surface.get_rect(center=rect.center)
    surface.blit(text_surface, text_rect)

def ttsTopLeft4(text, color, rect, surface, font_size, padding=20):
    font = pygame.font.Font(varGlobals.F_neue, font_size)
    text_surface = font.render(str(text), True, color)
    text_rect = text_surface.get_rect(left=(rect.left + padding))
    surface.blit(text_surface, text_rect)

def ttsMidLeft4(text, color, rect, surface, font_size, padding=20):
    font = pygame.font.Font(varGlobals.F_neue, font_size)
    text_surface = font.render(text, True, color)
    text_rect = text_surface.get_rect(midleft=(rect.left + padding, rect.centery))
    surface.blit(text_surface, text_rect)