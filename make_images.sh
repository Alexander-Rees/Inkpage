#!/usr/bin/bash

ID_FILE="./newspapers.csv"
TODAY_DAY=$(date +%-d)
TEMP_DIR="/tmp/frontpages"  

convert_pdf_to_bitmap() {

    local_pdf=$1  
    output_dir=$TEMP_DIR
    base_name=$(basename "$local_pdf" .pdf)

    # Define the paths for the output images
    local_1200_img="$output_dir/1200_${base_name}.png"
    local_825_img="$output_dir/825_${base_name}.png"

    # Extract width and height from the PDF
    pdf_width_points=$(pdfinfo "$local_pdf" | grep 'Page size' | awk '{print $3}')

    # Convert points to inches (1 point = 1/72 inches)
    pdf_width_inches=$(echo "$pdf_width_points / 72" | bc -l)

    # Calculate required densities
    density_1200=$(echo "1200 / $pdf_width_inches" | bc -l)
    density_825=$(echo "825 / $pdf_width_inches" | bc -l)

    echo ""
    # Convert PDF to bitmap using the calculated densities
    convert -density "$density_1200" "$local_pdf" -dither FloydSteinberg -colorspace Gray -colors 65535 -depth 8 "$local_1200_img"
    echo "Info: $(file "$local_1200_img")"

    convert -density "$density_825" "$local_pdf" -dither FloydSteinberg -colorspace Gray -colors 65535 -depth 8 "$local_825_img"
    echo "Info: $(file "$local_825_img")"

    echo ""
    echo "------------------------------------------------------------------------"

}

# check if frontpages dir got claimed by OS
if [ ! -e $TEMP_DIR ]; then
    echo "creating image directory: $TEMP_DIR"
    mkdir $TEMP_DIR
else
    echo "$TEMP_DIR exists, nothing to do"
fi


tail -n +1 "$ID_FILE" | while IFS=, read -r id rest; do
    echo "Processing ID: $id"
    NEWSPAPER_LINK="https://cdn.freedomforum.org/dfp/pdf$TODAY_DAY/$id.pdf"
    
    wget --spider "$NEWSPAPER_LINK"
    
    if [ $? -eq 0 ]; then
        echo "Successfully accessed $NEWSPAPER_LINK. Proceeding to download..."
        
        LOCAL_FILE_PATH="$TEMP_DIR/$id.pdf"
        
        wget -O "$LOCAL_FILE_PATH" "$NEWSPAPER_LINK"
        
        if [ -f "$LOCAL_FILE_PATH" ]; then
            echo "Downloaded $id PDF successfully to $LOCAL_FILE_PATH"
            convert_pdf_to_bitmap "$LOCAL_FILE_PATH"
        else
            echo "Failed to download $id PDF"
        fi
        
    else
        echo "Failed to access $NEWSPAPER_LINK"
    fi
    
done



