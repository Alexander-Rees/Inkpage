# InkPage

InkPage is a project for the [10 Inch Inkplate e-paper display](https://soldered.com/product/inkplate-10-9-7-e-paper-board-copy/ "10 Inch Inkplate") from [Soldered Electronics](https://soldered.com "Soldered"), with a resolution of 1200 x 825 pixels and 16 levels of gray (3-bit) or black and white (1-bit).
With InkPage, you can configure your Inkplate screen to show a rotating selection of daily current newspaper frontpages. There are over 400 newspapers from around the world available.

## 1. Frontend
Each frontpage can be shown for a set amount of minutes on the Inkplate's 10" e-ink screen. In addition, the orientation of the page can be set to either landscape or portrait. The settings are saved into `selected.json`. A flask app serves the newspaper selection and show configuration form and creates the `selected.json` file.

To change the newspaper selection, pick the newspapers of interest from `ff_frontpages.csv` and put them into **`newspapers.csv`**. They will then show up in the selection form.

## 2. Backend
The `make_images.sh` bash script is run via cron job to retrieve the latest newspaper frontpage PDF files from [Freedom Forum](http://freedomforum.org) and converts them to 16 level grayscale images that the Inkplate will render.

## 3. Inkplate client
`Inkpage_program.ino` is the C++ Arduino Sketch for the ESP32 microcontroller that fetches and renders the frontpage images, based on the `selected.json` file from the image server. You have to configure your own Wifi and image server here, then compile and upload the firmware with the [Arduino IDE](https://www.arduino.cc/en/software).

![WSJ](/static/wsj.jpeg)

![SJMN](/static/sjmn.jpeg)

https://github.com/Alexander-Rees/Inkpage/assets/120013251/3148c8b7-f919-43a6-9e8e-b44b144c69f5


https://github.com/Alexander-Rees/Inkpage/assets/120013251/967f7d0f-7323-411a-ad56-ba32395a8213

