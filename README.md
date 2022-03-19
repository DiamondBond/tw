# Timewave
Calculate the value of the timewave at a point.
Original Author: Peter Meyer (1998-01-05)

Changes compared to original:
  - include <ctype.h>
  - strlwr() function inlined; 37-47

Building:
```
  make clean
  make tw
```

Usage:
``
  TW days_to_zero_date days days ... [wf=nn].
  
  wf = wave factor (default 64, range 2-10000)
``
