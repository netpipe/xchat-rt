alias itunes {
    # get current artist
    set artist [exec osascript -e {tell application "iTunes" to return artist of current track}]
    
    set album [exec osascript -e {tell application "iTunes" to return name of current track}]
    set time [exec osascript -e {tell application "iTunes" to return time of current track}]
    /say "iTunes: $artist - $album ($time)"
    complete
}