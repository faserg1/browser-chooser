# Browser Chooser

You want to select, which browser to open, when clicked link in app, but automaticly? Mee too, so I made this program.

## Features

1. Console-only. Yeah, maybe it will look better with some windows and stuff... so, pull request will be cool :)
2. Small and fast. There is no other dependenties like .NET or some other stuff.
3. Portable (almost xD). Program will install it itself — just put it where you want it to be and then run it as administrator. Reboot computer and select Browser Chowser as a default browser.
4. Controle: you decide, what browser to open by next filters: protocol, domain, path

## Install

1. Place executable somewhere (so will not move it, and it will not disturbe you).
2. Run as administrator.
3. Reboot computer
4. Set Browser chooser as default browser

## Usage

### To list installed browsers:

```Batch file (DOS)
BrowserChooser.exe --list
```

### To List all filters

```Batch file (DOS)
BrowserChooser.exe --cfg @
```

### To Add filter

```Batch file (DOS)
BrowserChooser.exe --cfg +#FILTER
```

### To Edit filter

```Batch file (DOS)
BrowserChooser.exe --cfg =NUM#FILTER
```

### To Remove filter

```Batch file (DOS)
BrowserChooser.exe --cfg -NUM#FILTER
```

### Filter syntax
```
protocol\`domain\`path\`browser
```
Use asterisk (\*) for any.
Protocol will be fully compared.
Domain will be compared from the and.
Path wiil be compared from the start.

## Compile

There is a solution =) Here used C++20 standard.

## Future plans

* Control more? Like, parse, if needed and then open? Or look for a query? Scripting behaviour?
* Windows and tray. Select default browser for a while?
* Set as default browser in installation
* ...
