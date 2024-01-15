import csv
import json 
import sys
# from pprint import pprint as pretty_print
from flask import Flask, render_template, request, jsonify

newspapers = []
with open('newspapers.csv', 'r') as csvfile:
    csvreader = csv.reader(csvfile)

    for row in csvreader:
        newspaper_dict = {"id": row[0], "title": row[2]}
        newspapers.append(newspaper_dict)

app = Flask(__name__)

@app.route("/", methods=["GET", "POST"])
def index():
    count = request.form.get('count', default=1, type=int)
    max_dropdowns = 5

    request_data = str(request)

    app.logger.info(request_data)
    app.logger.info('Headers: %s', request.headers)
    app.logger.info('Body: %s', request.data)

    if request.method == "POST":
        
        selections = {}

        if 'orientation' in request.form:
            selections['orientation'] = request.form.get('orientation')
        if 'add' in request.form and count < max_dropdowns:
            count += 1
        elif 'submit' in request.form:
            
            for i in range(count):
                selected_id = request.form.get(f'titles_{i}')
                display_time = request.form.get(f'display_time_{i}')

                if selected_id and display_time:
                    selections[selected_id] = display_time

            with open("/tmp/frontpages/selected.json", "w") as json_file:
                json.dump(selections, json_file)
                
            return jsonify(selections)

    return render_template("index.html", newspapers=newspapers, count=count, max_dropdowns=max_dropdowns)

