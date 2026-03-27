import math

# All 176 levels: (position, level_id, name)
levels = [
(1, 119544028, "Thinking Space II"),
(2, 126242564, "Flamewall"),
(3, 119550490, "Amethyst"),
(4, 86407629, "Tidal Wave"),
(5, 133175713, "ORBIT"),
(6, 109780665, "Nullscapes"),
(7, 117692518, "Quanteuse processing"),
(8, 110816181, "BOOBAWAMBA"),
(9, 116174063, "Every End"),
(10, 114283297, "andromeda"),
(11, 127997391, "Subsuming Vortex"),
(12, 4125776, "Silent clubstep"),
(13, 112313819, "Anathema"),
(14, 62912799, "Ashley Wave Trials"),
(15, 89496627, "Avernus"),
(16, 73667628, "Acheron"),
(17, 110815379, "Spectre"),
(18, 107805281, "Menace"),
(19, 49896559, "Abyss of Darkness"),
(20, 120012581, "Defeated Circles"),
(21, 91351939, "Tunnel of Despair"),
(22, 86018142, "Kyouki"),
(23, 113599729, "Subterminal Point"),
(24, 27690100, "Slaughterhouse"),
(25, 87665224, "KOCMOC"),
(26, 110534288, "Based After Based"),
(27, 93917076, "The Lightning Rod"),
(28, 114281093, "CHIL"),
(29, 76962930, "Sakupen Circles"),
(30, 93091893, "Deimos (ItsHybrid)"),
(31, 95851008, "Eyes in the Water"),
(32, 131599104, "Voltage (ThePurgatory)"),
(33, 109439644, "KOSETSU"),
(34, 49072489, "Through The Gates"),
(35, 75206202, "Firework"),
(36, 113959291, "Silentlocked"),
(37, 85133223, "poocubed"),
(38, 120289520, "Snowbound"),
(39, 90477539, "Saul Goodman"),
(40, 113045735, "The Salt Factory"),
(41, 113322063, "CONVULSION"),
(42, 89414220, "MINUSdry"),
(43, 113443235, "Apocalyptic Trilogy"),
(44, 120255728, "Sevvend Clubstep"),
(45, 81139702, "The Hallucination"),
(46, 94359172, "COMBUSTION"),
(47, 96314787, "Deadlier Clubstep"),
(48, 89187968, "Edge of Destiny"),
(49, 90390075, "Solar Flare"),
(50, 104232191, "The Catacombs"),
(51, 86084399, "LIMBO"),
(52, 114990369, "Gaggatrondra"),
(53, 94969889, "Belladonna"),
(54, 62556400, "Codependence (Solo)"),
(55, 113256247, "Collapse (Nexel)"),
(56, 82544060, "Mayhem"),
(57, 131497860, "CHROMACAVE"),
(58, 113220284, "The Plunge"),
(59, 81011195, "walter white"),
(60, 119689515, "Infinite Chaos"),
(61, 114999625, "Operation Evolution"),
(62, 93339534, "Damascus"),
(63, 95719795, "Decks Dark"),
(64, 95998005, "SARYYX NEVER CLEAR"),
(65, 110500920, "The Yangire"),
(66, 95049815, "Climax"),
(67, 105593215, "ORDINARY"),
(68, 126571887, "Loops of Fury"),
(69, 128093374, "Wavterminal"),
(70, 82249742, "Sinister Silence"),
(71, 96083028, "Midnight"),
(72, 93340783, "Cimmerian Shade"),
(73, 103925676, "PSYCHOPATH"),
(74, 72315402, "arcturus"),
(75, 93792764, "BEELINE (Solo)"),
(76, 69685815, "Sonic Wave Infinity"),
(77, 59075347, "Tartarus"),
(78, 110991117, "Waterfall"),
(79, 108708033, "Coalescence"),
(80, 122941596, "THE JET ENGINE"),
(81, 95031870, "Jigsaw"),
(82, 94858072, "The Wonder of You"),
(83, 93917362, "Delta"),
(84, 109947627, "BPATA MPAKA"),
(85, 107741051, "Natural Disaster"),
(86, 90057148, "Terminal Rampancy"),
(87, 60978746, "The Golden"),
(88, 71025973, "Oblivion"),
(89, 107238250, "Viprin UFO"),
(90, 95176417, "NETWORK (Agat3)"),
(91, 76543324, "Verdant Landscape"),
(92, 97086864, "Levigo"),
(93, 88203501, "UNKNOWN"),
(94, 104672501, "ATOMIC CANNON Mk III"),
(95, 75286957, "Shukketsu"),
(96, 105748155, "Checked Steam"),
(97, 88442157, "Critical Heat"),
(98, 87130877, "WOBBLING MACHINE (Solo)"),
(99, 93732702, "Crystal Crusher (Solo)"),
(100, 110705712, "limbo but uwu ig idk"),
(101, 71885708, "Graceful"),
(102, 113364415, "paranoia (amplitron)"),
(103, 87071894, "The Paroxysm of Rage"),
(104, 112242564, "Blood Echo"),
(105, 79771070, "Aerial Gleam"),
(106, 127023313, "DISCONNECT"),
(107, 100990392, "VOID"),
(108, 115380769, "Starlit Stroll"),
(109, 80714349, "Henken"),
(110, 71434979, "Trueffet"),
(111, 120060187, "azure blast"),
(112, 58673581, "Kenos"),
(113, 65588448, "Fragile"),
(114, 128385946, "chrome hearts"),
(115, 114530859, "Starlight Summit"),
(116, 78435955, "Esfera"),
(117, 99495619, "Destruction 19"),
(118, 97705490, "Time Lapse"),
(119, 88611404, "NEUTRA"),
(120, 125617849, "Dark Dimension"),
(121, 112231282, "Swing Swing"),
(122, 105625871, "Guideless Goobering"),
(123, 72744364, "Hard Machine"),
(124, 108141323, "DISSONANCE"),
(125, 130183035, "Goober Rage Stage"),
(126, 52374843, "Zodiac"),
(127, 107299600, "Axinie"),
(128, 110913375, "Judgement Knights"),
(129, 97364686, "Lithium"),
(130, 69289606, "Widestep"),
(131, 123853954, "IRIS"),
(132, 95524621, "Crackhead Circles"),
(133, 96035641, "Scream Machine"),
(134, 74391640, "Keres"),
(135, 115313314, "Galeforce"),
(136, 75813671, "Lotus Flower"),
(137, 122389640, "in this"),
(138, 63996127, "Cold Sweat"),
(139, 95767653, "Frost Spirit"),
(140, 112046512, "Ascent"),
(141, 69333212, "Promethean"),
(142, 54953085, "Thinking Space"),
(143, 106522770, "Dry Out Copyable 2"),
(144, 58161496, "Renevant"),
(145, 96471171, "ConClusion"),
(146, 90104288, "We Are Not The Same (Solo)"),
(147, 94578424, "Trotil"),
(148, 118664383, "Disconnected Descent"),
(149, 94043690, "Instinct (Krazyman50)"),
(150, 88136707, "Sky Shredder"),
(151, 58355141, "Calculator Core"),
(152, 94156085, "shimmer (amplitron)"),
(153, 45866767, "Crimson Planet"),
(154, 88233955, "DIRECTIONS"),
(155, 78745224, "Ringy Paracosm"),
(156, 91180835, "ATOMIC CANNON Mk II"),
(157, 114340057, "Axiom Asterism"),
(158, 112586799, "Indivine"),
(159, 57600307, "Cognition"),
(160, 88822041, "Neon Skyline"),
(161, 90597202, "CORRODERE"),
(162, 76159410, "Cosmic Cyclone"),
(163, 81721025, "qoUEO"),
(164, 33498127, "SARY NEVER CLEAR"),
(165, 115151904, "ta1LSD0ll"),
(166, 93155813, "Rigel"),
(167, 111193316, "Scrubbabingo force"),
(168, 80178903, "Call Me Maybe"),
(169, 71912451, "RUST"),
(170, 88868682, "SAND SAILOR"),
(171, 110038200, "Gloxinia"),
(172, 58150142, "Akashic Records"),
(173, 111862459, "meow hard"),
(174, 52310333, "Lucid Nightmares"),
(175, 82172844, "Cobwebs"),
(176, 85612571, "RUTHLESS"),
]

# Anchor points: (aredl_rank, etoh_rating)
anchors = [
    (1, 14.07),
    (3, 14.05),
    (4, 14.00),
    (10, 13.85),
    (13, 13.80),
    (16, 13.70),
    (19, 13.66),
    (24, 13.48),
    (29, 13.45),
    (35, 13.30),
    (37, 13.27),
    (44, 13.08),
    (58, 13.00),
    (72, 12.81),
    (87, 12.74),
    (95, 12.66),
    (110, 12.52),
    (112, 12.47),
    (124, 12.43),
    (144, 12.41),
    (162, 12.30),
    (179, 12.17),
    (188, 12.12),
    (208, 12.00),
    (266, 11.80),
    (323, 11.71),
    (423, 11.43),
    (455, 11.39),
    (491, 11.34),
    (659, 10.83),
    (740, 10.63),
    (878, 10.60),
    (1304, 10.25),
    (1388, 10.00),
]

def interpolate(rank):
    if rank <= anchors[0][0]:
        return anchors[0][1]
    if rank >= anchors[-1][0]:
        return anchors[-1][1]
    for i in range(len(anchors) - 1):
        r1, v1 = anchors[i]
        r2, v2 = anchors[i+1]
        if r1 <= rank <= r2:
            t = (rank - r1) / (r2 - r1)
            return v1 + t * (v2 - v1)
    return anchors[-1][1]

def get_tier(floor_val):
    if floor_val == 10:
        return "Terrifying"
    elif floor_val == 11:
        return "Catastrophic"
    elif floor_val == 12:
        return "Horrific"
    elif floor_val == 13:
        return "Unreal"
    elif floor_val >= 14:
        return "Nil"
    return "Unknown"

def get_sub(decimal_part):
    if decimal_part < 0.04:
        return "Baseline"
    elif decimal_part < 0.10:
        return "Bottom"
    elif decimal_part < 0.25:
        return "Bottom-Low"
    elif decimal_part < 0.38:
        return "Low"
    elif decimal_part < 0.50:
        return "Low-Mid"
    elif decimal_part < 0.60:
        return "Mid"
    elif decimal_part < 0.68:
        return "Mid-High"
    elif decimal_part < 0.78:
        return "High"
    elif decimal_part < 0.92:
        return "High-Peak"
    else:
        return "Peak"

for pos, lid, name in levels:
    raw = interpolate(pos)
    rating = round(raw, 2)
    floor_val = math.floor(rating)
    decimal_part = round(rating - floor_val, 10)  # avoid float imprecision
    tier = get_tier(floor_val)
    sub = get_sub(decimal_part)
    subtier = f"{sub} {tier}"
    print(f"    {{{lid}, {{{rating:.2f}f, \"{subtier}\", {pos}}}}},  // {name}")
