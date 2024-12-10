{\rtf1\ansi\ansicpg1252\cocoartf2818
\cocoatextscaling0\cocoaplatform0{\fonttbl\f0\fnil\fcharset0 Menlo-Bold;\f1\fnil\fcharset0 Menlo-Regular;}
{\colortbl;\red255\green255\blue255;\red46\green174\blue187;\red0\green0\blue0;\red180\green36\blue25;
\red47\green180\blue29;\red200\green20\blue201;\red64\green11\blue217;}
{\*\expandedcolortbl;;\cssrgb\c20199\c73241\c78251;\csgray\c0;\cssrgb\c76411\c21697\c12527;
\cssrgb\c20241\c73898\c14950;\cssrgb\c83397\c23074\c82666;\cssrgb\c32309\c18666\c88229;}
\margl1440\margr1440\vieww34360\viewh19220\viewkind0
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\pardirnatural\partightenfactor0

\f0\b\fs22 \cf2 \CocoaLigature0 from
\f1\b0 \cf3  flask 
\f0\b \cf2 import
\f1\b0 \cf3  Flask, request, jsonify, render_template\

\f0\b \cf2 import
\f1\b0 \cf3  time\
\
app = Flask(__name__)\
\

\f0\b \cf4 # Store received data in memory for demonstration purposes
\f1\b0 \cf3 \
stored_data = \{
\f0\b \cf5 "ambientTemp"
\f1\b0 \cf3 : 
\f0\b \cf6 None
\f1\b0 \cf3 , 
\f0\b \cf5 "objectTemp"
\f1\b0 \cf3 : 
\f0\b \cf6 None
\f1\b0 \cf3 , 
\f0\b \cf5 "timestamp"
\f1\b0 \cf3 : time.time()\}\
\
@app.route(
\f0\b \cf5 "/"
\f1\b0 \cf3 )\

\f0\b \cf2 def\cf7  index
\f1\b0 \cf3 ():\
    
\f0\b \cf2 return
\f1\b0 \cf3  render_template(
\f0\b \cf5 "index.html"
\f1\b0 \cf3 )\
\
@app.route(
\f0\b \cf5 "/data"
\f1\b0 \cf3 , methods=[
\f0\b \cf5 "GET"
\f1\b0 \cf3 , 
\f0\b \cf5 "POST"
\f1\b0 \cf3 ])\

\f0\b \cf2 def\cf7  receive_data
\f1\b0 \cf3 ():\
    
\f0\b \cf2 global
\f1\b0 \cf3  stored_data\
\
    
\f0\b \cf2 if
\f1\b0 \cf3  request.method == 
\f0\b \cf5 "POST"
\f1\b0 \cf3 :\
        
\f0\b \cf2 try
\f1\b0 \cf3 :\
           
\f0\b \cf4  # Parse the incoming JSON data
\f1\b0 \cf3 \
            data = request.get_json()\
            
\f0\b \cf2 if
\f1\b0 \cf3  
\f0\b \cf2 not
\f1\b0 \cf3  data:\
                
\f0\b \cf2 return
\f1\b0 \cf3  jsonify(\{
\f0\b \cf5 "error"
\f1\b0 \cf3 : 
\f0\b \cf5 "No JSON payload received"
\f1\b0 \cf3 \}), 400\
\
           
\f0\b \cf4  # Update stored data with received temperature values
\f1\b0 \cf3 \
            stored_data[
\f0\b \cf5 "ambientTemp"
\f1\b0 \cf3 ] = data.get(
\f0\b \cf5 "ambientTemp"
\f1\b0 \cf3 )\
            stored_data[
\f0\b \cf5 "objectTemp"
\f1\b0 \cf3 ] = data.get(
\f0\b \cf5 "objectTemp"
\f1\b0 \cf3 )\
            stored_data[
\f0\b \cf5 "timestamp"
\f1\b0 \cf3 ] = time.time() 
\f0\b \cf4  # Update timestamp
\f1\b0 \cf3 \
\
            print(f
\f0\b \cf5 "Received Ambient Temperature: \{stored_data['ambientTemp']\} \'b0C"
\f1\b0 \cf3 )\
            print(f
\f0\b \cf5 "Received Object Temperature: \{stored_data['objectTemp']\} \'b0C"
\f1\b0 \cf3 )\
\
            
\f0\b \cf2 return
\f1\b0 \cf3  jsonify(\{
\f0\b \cf5 "message"
\f1\b0 \cf3 : 
\f0\b \cf5 "Data received successfully"
\f1\b0 \cf3 \}), 200\
        
\f0\b \cf2 except
\f1\b0 \cf3  Exception 
\f0\b \cf2 as
\f1\b0 \cf3  e:\
            print(f
\f0\b \cf5 "Error: \{e\}"
\f1\b0 \cf3 )\
            
\f0\b \cf2 return
\f1\b0 \cf3  jsonify(\{
\f0\b \cf5 "error"
\f1\b0 \cf3 : 
\f0\b \cf5 "Something went wrong"
\f1\b0 \cf3 \}), 500\
    
\f0\b \cf2 else
\f1\b0 \cf3 :\
       
\f0\b \cf4  # Handle GET requests by returning the data
\f1\b0 \cf3 \
        
\f0\b \cf2 return
\f1\b0 \cf3  jsonify([\{\
            
\f0\b \cf5 "timestamp"
\f1\b0 \cf3 : stored_data[
\f0\b \cf5 "timestamp"
\f1\b0 \cf3 ],\
            
\f0\b \cf5 "ambientTemp"
\f1\b0 \cf3 : stored_data[
\f0\b \cf5 "ambientTemp"
\f1\b0 \cf3 ],\
            
\f0\b \cf5 "objectTemp"
\f1\b0 \cf3 : stored_data[
\f0\b \cf5 "objectTemp"
\f1\b0 \cf3 ]\
        \}])\
\

\f0\b \cf2 if
\f1\b0 \cf3  __name__ == 
\f0\b \cf5 "__main__"
\f1\b0 \cf3 :\
    app.run(host=
\f0\b \cf5 "0.0.0.0"
\f1\b0 \cf3 , port=5000, debug=
\f0\b \cf6 True
\f1\b0 \cf3 )}