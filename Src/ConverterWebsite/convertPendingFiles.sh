#!/bin/bash -u

# Polyline Shapefile queue
cd /var/www/acadconverter.chrismichaelis.com/uploads_Polyline
for file in /var/www/acadconverter.chrismichaelis.com/uploads_Polyline/*.d*; do
	[[ "$file" =~ .dxf$ || "$file" =~ dwg$ ]] && {
		base=$(basename "$file")
		base=${base%.dwg}
		base=${base%.dxf}
		/var/www/acadconverter.chrismichaelis.com/bin/AutoCADConverter "$file" "$base.shp" line
		[ -e "../outputs/$base.zip" ] && rm -f "../outputs/$base.zip"
		zip "../outputs/$base.zip" "$base.shx" "$base.shp" "$base.dbf" "${base}_text.shp" "${base}_text.shx" "${base}_text.dbf"
		zip "../uploads_done/$(date +%s)_$base.zip" "$base.shx" "$base.shp" "$base.dbf" "${base}_text.shp" "${base}_text.shx" "${base}_text.dbf" "$file"
		rm -f "$file" "$base.shx" "$base.shp" "$base.dbf" "${base}_text.shp" "${base}_text.shx" "${base}_text.dbf"
	}
done

# Polygon Shapefile queue
cd /var/www/acadconverter.chrismichaelis.com/uploads_Polygon
for file in /var/www/acadconverter.chrismichaelis.com/uploads_Polygon/*.d*; do
	[[ "$file" =~ .dxf$ || "$file" =~ dwg$ ]] && {
		base=$(basename "$file")
		base=${base%.dwg}
		base=${base%.dxf}
		/var/www/acadconverter.chrismichaelis.com/bin/AutoCADConverter "$file" "$base.shp" polygon 
		[ -e "../outputs/$base.zip" ] && rm -f "../outputs/$base.zip"
		zip "../outputs/$base.zip" "$base.shx" "$base.shp" "$base.dbf" "${base}_text.shp" "${base}_text.shx" "${base}_text.dbf"
		zip "../uploads_done/$(date +%s)_$base.zip" "$base.shx" "$base.shp" "$base.dbf" "${base}_text.shp" "${base}_text.shx" "${base}_text.dbf" "$file"
		rm -f "$file" "$base.shx" "$base.shp" "$base.dbf" "${base}_text.shp" "${base}_text.shx" "${base}_text.dbf"
	}
done

# Point Shapefile queue
cd /var/www/acadconverter.chrismichaelis.com/uploads_Point
for file in /var/www/acadconverter.chrismichaelis.com/uploads_Point/*.d*; do
	[[ "$file" =~ .dxf$ || "$file" =~ dwg$ ]] && {
		base=$(basename "$file")
		base=${base%.dwg}
		base=${base%.dxf}
		/var/www/acadconverter.chrismichaelis.com/bin/AutoCADConverter "$file" "$base.shp" point
		[ -e "../outputs/$base.zip" ] && rm -f "../outputs/$base.zip"
		zip "../outputs/$base.zip" "$base.shx" "$base.shp" "$base.dbf" "${base}_text.shp" "${base}_text.shx" "${base}_text.dbf"
		zip "../uploads_done/$(date +%s)_$base.zip" "$base.shx" "$base.shp" "$base.dbf" "${base}_text.shp" "${base}_text.shx" "${base}_text.dbf" "$file"
		rm -f "$file" "$base.shx" "$base.shp" "$base.dbf" "${base}_text.shp" "${base}_text.shx" "${base}_text.dbf"
	}
done

# KML queue
cd /var/www/acadconverter.chrismichaelis.com/uploads_KML
for file in /var/www/acadconverter.chrismichaelis.com/uploads_KML/*.d*; do
	[[ "$file" =~ .dxf$ || "$file" =~ dwg$ ]] && {
		base=$(basename "$file")
		base=${base%.dwg}
		base=${base%.dxf}
		/var/www/acadconverter.chrismichaelis.com/bin/AutoCADConverter "$file" "$base.kml" 
		[ -e "../outputs/$base.zip" ] && rm -f "../outputs/$base.zip"
		zip "../outputs/$base.zip" "$base.kml"
		zip "../uploads_done/$(date +%s)_$base.zip" "$base.kml" "$file"
		rm -f "$file" "$base.kml"
	}
done

# Text queue
cd /var/www/acadconverter.chrismichaelis.com/uploads_Text
for file in /var/www/acadconverter.chrismichaelis.com/uploads_Text/*.d*; do
	[[ "$file" =~ .dxf$ || "$file" =~ dwg$ ]] && {
		base=$(basename "$file")
		base=${base%.dwg}
		base=${base%.dxf}
		/var/www/acadconverter.chrismichaelis.com/bin/AutoCADConverter "$file" "$base.txt" 
		[ -e "../outputs/$base.zip" ] && rm -f "../outputs/$base.zip"
		zip "../outputs/$base.zip" "$base.txt"
		zip "../uploads_done/$(date +%s)_$base.zip" "$base.txt" "$file"
		rm -f "$file" "$base.txt"
	}
done
