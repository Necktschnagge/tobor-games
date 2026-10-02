# ---------------------------------------------------------------------
#  Code-Formatting  -  clang-format  -  format changed files only
# ---------------------------------------------------------------------
#
#  This script resursively formats all files which fulfill all of the following conditions:
#     - File matches file extension list: (e.g. `.cpp`, `.h`)
#     - File path does not match exclusion list: (e.g. `extern/*`)
#     - File has changed since reference commit (given as argument or default)
#



# ----------------------------------------
# Configuration
# ----------------------------------------
Write-Output "Configuration:"

#$sourcesRootDir = "."
$sourcesRootDir = ".."

Push-Location .
Set-Location $sourcesRootDir

#$gitComparisonTarget = "origin/master"
#$gitComparisonTarget = "origin/main"
#$gitComparisonTarget = "HEAD~5" # for local development: compare to 5 commits ago
$gitComparisonTarget = $(git rev-list --max-parents=0 HEAD) # the very first commit (root commit)

# clang-format path
#$clangFormatPath = "clang-format.exe"
$clangFormatPath = "C:\Program Files\LLVM\bin\clang-format.exe"

# File extensions to format
$extensions = @(".c", ".cpp", ".h", ".hpp", ".ipp", ".i")

# Regex patterns for files to ignore
$ignoredPatterns = @(
    '^extern/.*$'
)

Write-Output " Default properties:"
Write-Output " ------------------ "
Write-Output "            sourcesRootDir = ${sourcesRootDir}"
Write-Output "            sourcesRootDir = $((Get-Item .).FullName)"
Write-Output "       gitComparisonTarget = ${gitComparisonTarget}"
Write-Output "           clangFormatPath = ${clangFormatPath}"
Write-Output "                extensions = ${extensions}"
Write-Output "           ignoredPatterns = ${ignoredPatterns}"

Write-Output ""
Write-Output "  Custom properties:"
Write-Output " ------------------ "

# Track failures
$failed = 0



# ----------------------------------------
# Determine gitComparisonTarget
# ----------------------------------------
if ($args.Count -ge 1) {
    $gitComparisonTarget = $args[0]
    Write-Output "       gitComparisonTarget = ${gitComparisonTarget}"
}

Write-Output ""
Write-Output "Processing..."

Write-Output "Running git to find all changed files..."

# --------------------------------------------
# Get files changed since gitComparisonTarget
# --------------------------------------------
$files = git diff --name-only $gitComparisonTarget

Write-Output ${files}
Write-Output "Running git to find all changed files   ...DONE"

# Filter out non-existent files (deleted, renamed, etc.)
$files = $files | Where-Object { Test-Path $_ }

# Filter out ignored files
foreach ($pattern in $ignoredPatterns) {
    $files = $files | Where-Object { $_ -notmatch $pattern }
}



# ----------------------------------------
# Run formatter on matching files
# ----------------------------------------
foreach ($ext in $extensions) {
    # Match files ending with the extension
    $regex = [regex]::Escape($ext) + '$'
    $matchingFiles = $files | Where-Object { $_ -match $regex }

    foreach ($file in $matchingFiles) {
		# Print which file is being formatted
		Write-Host "Formatting: $file"
		
        & $clangFormatPath -i $file
        if ($LASTEXITCODE -ne 0) {
            $failed = 1
        }
    }
}



# ----------------------------------------
# Exit with accumulated failure status
# ----------------------------------------
Pop-Location
exit $failed
