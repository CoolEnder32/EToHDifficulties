$allLevels = @()
$page = 1
do {
    $url = "https://api.aredl.net/api/aredl/levels?page=" + $page + "&limit=200"
    try {
        $r = Invoke-WebRequest -Uri $url -UseBasicParsing
        $j = $r.Content | ConvertFrom-Json
        $count = $j.Count
        Write-Host ("Page " + $page + ": " + $count + " entries")
        if ($count -eq 0) { break }
        $allLevels += $j
        if ($count -lt 200) { break }
        $page++
    } catch {
        Write-Host ("Error on page " + $page)
        break
    }
} while ($true)

Write-Host ("Total levels: " + $allLevels.Count)
$lines = @("position,level_id,name")
foreach ($lvl in $allLevels) {
    $safeName = $lvl.name -replace '"', '""'
    $lines += ($lvl.position.ToString() + "," + $lvl.level_id.ToString() + ',"' + $safeName + '"')
}
$lines | Out-File -FilePath "c:\Users\hello\EtohDifficulties\aredl_levels.csv" -Encoding UTF8
Write-Host "Written to aredl_levels.csv"
