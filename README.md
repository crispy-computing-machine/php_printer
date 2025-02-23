# PHP Printer

A lightweight PHP library for seamless printer interaction on Windows systems. This project leverages native Windows APIs to enable direct printing capabilities from PHP applications, making it ideal for developers building web or command-line tools that require printer integration.

## Features
- Direct access to local printers from PHP.
- Supports both web and CLI applications.
- Simple API for sending print jobs to the default printer.

## Requirements
- **PHP Version**: 8 or higher (tested up to PHP 8.4).
- **Operating System**: Windows XP or later.
- **PHP Extension**: `php_printer.dll` must be installed and enabled in your `php.ini`.

## Installation

### Step 1: Install PHP and Enable the Printer Extension
1. Ensure you have a working PHP installation (5.3.x or higher).
2. Download the `php_printer.dll` from the releases on this repo.
3. Place the `php_printer.dll` file in your PHP extension directory (e.g., `C:\php\ext`).
4. Edit your `php.ini` file to enable the extension:
