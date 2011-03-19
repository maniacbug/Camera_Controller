# wget -O - http://spaceflightnow.com/shuttle/sts134/fdf/134windows.html | html2text -width 300 | gawk -f launch_transform.awk -v name=sts134 > sts134_windows.cpp

function trim0(str)
{
  return gensub(/0([1-9])/," \\1","g",str)
}

BEGIN {
  FS="[. ]*"
  OFS = " "
  print "#include \"windows.h\""
  print "// Launch window definitions for mission #" name
  print "window " name "_windows[] = { ";
}

/3$/ {
  j = 0
  for (i=1;i<=NF;i++)
    if ( match($i,/^[0-9][0-9]\/[0-9][0-9]\/[0-9][0-9]$/) )
      j = i
      
  gsub(/[:/]/,",")
  if (j>0)
  {
    ORS=","
    print "{ {" trim0($j) "}"; ++j
    print "{" trim0($j); ++j
    print $j "}"; ++j
    print "{" trim0($j); ++j
    print $j "}"; ++j
    print "{" trim0($j); ++j
    print $j "} }"; ++j
    ORS="\n"
    print ""
  }
}

END {
  print "};"
  print "const int num_" name "_windows = sizeof(" name "_windows)/sizeof(window);"
}
