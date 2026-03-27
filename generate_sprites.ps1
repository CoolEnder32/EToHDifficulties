$lines = @()
$lines += "#pragma once"
$lines += ""

$sprites = @(
    @{ file = "easy";     diff = "Easy"         },
    @{ file = "medium";   diff = "Medium"        },
    @{ file = "hard";     diff = "Hard"          },
    @{ file = "diff";     diff = "Difficult"     },
    @{ file = "chall";    diff = "Challenging"   },
    @{ file = "intense";  diff = "Intense"       },
    @{ file = "rem";      diff = "Remorseless"   },
    @{ file = "insane";   diff = "Insane"        },
    @{ file = "extreme";  diff = "Extreme"       },
    @{ file = "terri";    diff = "Terrifying"    },
    @{ file = "cata";     diff = "Catastrophic"  },
    @{ file = "horri";    diff = "Horrific"      },
    @{ file = "unreal";   diff = "Unreal"        },
    @{ file = "nil";      diff = "Nil"           }
)

foreach ($s in $sprites) {
    $path = "resources\$($s.file).png"
    if (-not (Test-Path $path)) {
        Write-Error "Missing: $path"
        exit 1
    }
    $bytes = [System.IO.File]::ReadAllBytes($path)
    $hex   = ($bytes | ForEach-Object { "0x{0:X2}" -f $_ }) -join ","
    $id    = $s.file
    $lines += "static const unsigned char k_spr_${id}[] = {$hex};"
    $lines += "static const int           k_spr_${id}_size = $($bytes.Count);"
    $lines += ""
    Write-Host "Embedded $path ($($bytes.Count) bytes)"
}

[System.IO.File]::WriteAllLines("src\sprites_data.hpp", $lines)
Write-Host "Done -> src\sprites_data.hpp"
