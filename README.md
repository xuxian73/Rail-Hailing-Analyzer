# Rail-Hailing Analyzer

This is a concise and efficient visualization of ride-hailing order data in Chengdu. The project is developed based on Qt.
Look for more detail in the PDF file, Rail-Hail-Analyzer report.

## Build

Simply unzip the .dmg file and run on macOS is ok.

Use qmake to build the project. Replace $QT_ROOT_PATH with the path to Qt5.14.2 and replace the $FOLDER_PATH with the path to the project folder. 

~~~
$QT_ROOT_PATH/Qt5.14.2/5.14.2/clang_64/bin/qmake $FOLDER_PATH/Ride-Hailing-Analysis/Ride-Hailing-Analysis.pro -spec macx-clang CONFIG+=x86_64 CONFIG+=qtquickcompiler && /usr/bin/make qmake_all
~~~

## Module

There are three main modules in this application.

### Spatio-temporal chart of order

Users are able to query a line chart of orders per hour over time. Users are also allowed to tune the time range, time step and grid ID.

### Travel time and fee distribution

Users are able to query a histogram chart of travel time and fee.Users are allowed to tune the time range.

### Related order search

Users are able to input some order infomation, like the departure and destination position and departure time, the app would return the most related order in the database. Users can visualize the data numerically in a table or simply on the map.

## Performance

| Test Event                                          | TIme Consuming(ms) |
| --------------------------------------------------- | ------------------ |
| Build Database(1 day)                               | 1348.0             |
| Build Database(15 days)                             | 21378.2            |
| Plot order-time chart(all grids, 1 day)             | 594.4              |
| Plot order-time chart(all grids, 15 days)           | 17836.8            |
| Distribution Analysis(travel time, 1 day)           | 768.5              |
| Distribution Analysis(fee, 15 day)                  | 11439.8            |
| Related Order Searching(seardch in one-day dataset) | 93                 |
| Related Order Searching(search in all datasets)     | 785                |

