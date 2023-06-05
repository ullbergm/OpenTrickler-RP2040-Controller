#include <string.h>
#include <stdlib.h>
#include "rest_endpoints.h"
#include "http_rest.h"
#include "charge_mode.h"
#include "motors.h"
#include "scale.h"
#include "wireless.h"
#include "eeprom.h"
#include "rotary_button.h"
#include "display.h"


// Following HTMLs are generated by html2header.py under scripts
const char html_display_mirror[] = "<!doctype html><title>Pixel Renderer</title><style>canvas{border:1px solid black}</style><body><canvas id=pixel-canvas></canvas><script>const canvas=document.getElementById(\"pixel-canvas\");const context=canvas.getContext(\"2d\");function renderPixel(a,b,c){context.fillStyle=c;context.fillRect(a,b,4,4)}const scaleFactor=4;canvas.width=128*scaleFactor;canvas.height=64*scaleFactor;function fetchAndRender(){fetch(\"/display_buffer\").then(a=>a.arrayBuffer()).then(a=>{const b=new Uint8Array(a);const c=0x10;for(let d=0;d<8;d++){for(let e=0;e<8;e++){for(let f=0;f<c*8;f++){const g=f+ d*c*8;let h;try{h=b[g]}catch(j){console.log(g);throw j};const i=1<<e&h?\"black\":\"white\";renderPixel(f*scaleFactor,d*8*scaleFactor+ e*scaleFactor,i)}}}}).catch(a=>{console.log(\"Error fetching binary data:\",a)})}setInterval(fetchAndRender,1000)</script>";

const char html_plot_weight[] = "<!doctype html><title>Scale Monitoring</title><script src=https://cdn.plot.ly/plotly-latest.min.js></script><script src=https://code.jquery.com/jquery-3.6.0.min.js></script><body><h1>Scale Monitoring</h1><h2>Current Weight: <span id=currentWeight>Loading...</span></h2><div id=chart></div><script>var plotData={x:[],y:[],mode:\'lines\',type:\'scatter\'};var layout={title:\'Weight over Time\',xaxis:{title:\'Time\'},yaxis:{title:\'Weight\'}};Plotly.newPlot(\'chart\',[plotData],layout);function fetchScaleWeight(){$.ajax({url:\'/rest/scale_weight\',type:\'GET\',dataType:\'json\',success:function(a){document.getElementById(\'currentWeight\').textContent=a.weight.toFixed(3);var b=new Date().getTime();plotData.x.push(b);plotData.y.push(a.weight.toFixed(3));var c=b- 20000;while(plotData.x[0]<c){plotData.x.shift();plotData.y.shift()};Plotly.update(\'chart\',[plotData],layout)},error:function(a,b,c){console.error(\'Error fetching scale weight:\',c)},complete:function(){setTimeout(fetchScaleWeight,500)}})}fetchScaleWeight()</script>";

const char html_config[] = "<!doctype html><title>Settings</title><link href=https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css rel=stylesheet><link href=https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css rel=stylesheet><style>body{background-color:#f2f2f2;padding:20px}h1{color:#333;text-align:center}.container{display:flex}.nav-column{flex:1;padding-right:20px}.config-column{flex:2}.nav-link{display:block;padding:10px;background-color:#fff;border-radius:5px;margin-bottom:10px;color:#333;text-decoration:none}.nav-link:hover {background-color:#f5f5f5}.nav-link.active{background-color:#4caf50;color:#fff}.form-group label{color:#555}.form-group input,.form-group select{width:100%}.btn-primary{background-color:#4caf50;border-color:#4caf50}.label{color:white;padding:2px}.success{background-color:#04aa6d}.info{background-color:#2196f3}.warning{background-color:#ff9800}.danger{background-color:#f44336}.other{background-color:#e7e7e7;color:black}</style><body><h1>Open Trickler Settings</h1><div class=container><div class=nav-column><a class=\"nav-link active\"href=#scale_config>Scale</a><a class=nav-link href=#charge_mode_config>Charge Mode</a><a class=nav-link href=#wireless_config>Wireless</a><a class=nav-link href=#coarse_motor_config>Coarse Motor</a><a class=nav-link href=#fine_motor_config>Fine Motor</a><a class=nav-link href=#system_control>System Control</a></div><div class=config-column><div class=config-section id=scale_config><h2>Scale Configuration</h2><form class=config-form id=scale_config-form><div class=form-group><label for=unit>Scale Unit:</label><select class=form-control id=unit name=unit><option value=grain>Grain<option value=gram>Gram</select></div><button class=\"btn btn-primary\"data-api=/scale_config>Apply Scale Settings</button></form></div><div class=config-section id=charge_mode_config style=display:none><h2>Charge Mode Configuration</h2><form class=config-form id=charge_mode_config-form><div class=form-group><label for=coarse_kp>Coarse Kp:</label><input class=form-control id=coarse_kp name=coarse_kp step=0.001 type=number></div><div class=form-group><label for=coarse_ki>Coarse Ki:</label><input class=form-control id=coarse_ki name=coarse_ki step=0.001 type=number></div><div class=form-group><label for=coarse_kd>Coarse Kd:</label><input class=form-control id=coarse_kd name=coarse_kd step=0.001 type=number></div><div class=form-group><label for=fine_kd>Coarse Kd:</label><input class=form-control id=fine_kd name=fine_kd step=0.001 type=number></div><div class=form-group><label for=fine_ki>Coarse Kd:</label><input class=form-control id=fine_ki name=fine_ki step=0.001 type=number></div><div class=form-group><label for=fine_kd>Coarse Kd:</label><input class=form-control id=fine_kd name=fine_kd step=0.001 type=number></div><button class=\"btn btn-primary\"data-api=/charge_mode_config>Apply Charge Mode Settings</button></form></div><div class=config-section id=wireless_config style=display:none><h2>Wireless Configuration</h2><form class=config-form id=wireless_config-form><div class=form-group><label for=ssid>WiFi SSID:</label><input class=form-control id=ssid name=ssid></div><div class=form-group><label for=pw>WiFi Password:</label><input class=form-control id=pw name=pw type=password></div><div class=form-group><label for=auth>WiFi Authentication Method:</label><select class=form-control id=auth name=auth><option value=CYW43_AUTH_OPEN>None<option value=CYW43_AUTH_WPA_TKIP_PSK>WPA-TKIP<option value=CYW43_AUTH_WPA2_AES_PSK>WPA2-AES<option value=CYW43_AUTH_WPA2_MIXED_PSK>WPA2-MIXED</select></div><div class=form-group><label for=configured>Use WiFi:</label><select class=form-control id=configured name=configured><option value=true>Yes<option value=false>No</select></div><button class=\"btn btn-primary\"data-api=/wireless_config>Apply Wireless Settings</button></form></div><div class=config-section id=coarse_motor_config style=display:none><h2>Coarse Motor Configuration</h2><form class=config-form id=coarse_motor_config-form><div class=form-group><label for=angular_acceleration>Angular Acceleration (rps/s):</label><input class=form-control id=angular_acceleration name=angular_acceleration step=0.001 type=number></div><div class=form-group><label for=full_steps_per_rotation>Full Steps per Rotation:</label><select class=form-control id=full_steps_per_rotation name=full_steps_per_rotation><option value=200>200<option value=400>400</select></div><div class=form-group><label for=current_ma>Current (ma):</label><input class=form-control id=current_ma name=current_ma step=1 type=number></div><div class=form-group><label for=microsteps>Microsteps:</label><select class=form-control id=microsteps name=microsteps><option value=16>16<option value=32>32<option value=64>64<option value=128>128<option value=256>256</select></div><div class=form-group><label for=max_speed_rps>Max Speed (rps):</label><input class=form-control id=max_speed_rps name=max_speed_rps step=0.1 type=number></div><div class=form-group><label for=r_sense>Current Sensing Resistor (mOhm):</label><input class=form-control id=r_sense name=r_sense step=1 type=number></div><div class=form-group><label for=inv_en>Inverted Enable Pin:</label><select class=form-control id=inv_en name=inv_en><option value=true>Yes<option value=false>No</select></div><div class=form-group><label for=inv_dir>Inverted Step Direction Pin:</label><select class=form-control id=inv_dir name=inv_dir><option value=true>Yes<option value=false>No</select></div><button class=\"btn btn-primary\"data-api=/coarse_motor_config>Apply Coarse Motor Settings</button></form></div><div class=config-section id=fine_motor_config style=display:none><h2>Fine Motor Configuration</h2><form class=config-form id=fine_motor_config-form><div class=form-group><label for=angular_acceleration>Angular Acceleration (rps/s):</label><input class=form-control id=angular_acceleration name=angular_acceleration step=0.001 type=number></div><div class=form-group><label for=full_steps_per_rotation>Full Steps per Rotation:</label><select class=form-control id=full_steps_per_rotation name=full_steps_per_rotation><option value=200>200<option value=400>400</select></div><div class=form-group><label for=current_ma>Current (ma):</label><input class=form-control id=current_ma name=current_ma step=1 type=number></div><div class=form-group><label for=microsteps>Microsteps:</label><select class=form-control id=microsteps name=microsteps><option value=16>16<option value=32>32<option value=64>64<option value=128>128<option value=256>256</select></div><div class=form-group><label for=max_speed_rps>Max Speed (rps):</label><input class=form-control id=max_speed_rps name=max_speed_rps step=0.1 type=number></div><div class=form-group><label for=r_sense>Current Sensing Resistor (mOhm):</label><input class=form-control id=r_sense name=r_sense step=1 type=number></div><div class=form-group><label for=inv_en>Inverted Enable Pin:</label><select class=form-control id=inv_en name=inv_en><option value=true>Yes<option value=false>No</select></div><div class=form-group><label for=inv_dir>Inverted Step Direction Pin:</label><select class=form-control id=inv_dir name=inv_dir><option value=true>Yes<option value=false>No</select></div><button class=\"btn btn-primary\"data-api=/fine_motor_config>Apply Fine Motor Settings</button></form></div><div class=config-section id=system_control style=display:none><h2>System Control</h2><form class=config-form id=system_control-form><div class=form-group><label for=unique_id>Unique ID:</label><input class=form-control disabled id=unique_id name=unique_id readonly></div><div class=form-group><label for=save_to_eeprom>Save to EEPROM:</label><select class=form-control id=save_to_eeprom name=save_to_eeprom><option value=true>Yes<option value=false>No</select></div><div class=form-group><label for=software_reset>Reboot:</label><select class=form-control id=software_reset name=software_reset><option value=true>Yes<option value=false>No</select></div><div class=form-group><span class=\"label warning\">Warning</span><label for=erase_eeprom>Erase EEPROM:</label><select class=form-control id=erase_eeprom name=erase_eeprom><option value=true>Yes<option value=false>No</select></div><button class=\"btn btn-primary\"data-api=/system_control>Apply System Control</button></form></div></div></div><script src=https://code.jquery.com/jquery-3.6.0.min.js></script><script src=https://stackpath.bootstrapcdn.com/bootstrap/4.5.2/js/bootstrap.min.js></script><script>$(document).ready(function(){var a=$(\'.nav-link\').first().attr(\'href\');b(a);$(\'.nav-link\').click(function(e){e.preventDefault();var f=$(this).attr(\'href\');$(\'.nav-link\').removeClass(\'active\');$(this).addClass(\'active\');$(\'.config-section\').hide();$(f).show();b(f)});$(\'.config-form\').submit(function(e){e.preventDefault();var f=$(this).serialize();var g=$(this).find(\'button\').data(\'api\');d(f,g)});function b(e){var f=e.substr(1);var g=\'/rest/\'+ f;$.ajax({url:g,dataType:\'json\',method:\'GET\',success:function(h){c(f,h)},error:function(h,i,j){console.error(\'An error occurred while retrieving options:\',j)}})}function c(e,f){var g=$(\'#\'+ e+ \'-form\');for(var h in f){var i=f[h];var j=g.find(\'[name=\"\'+ h+ \'\"]\');j.val(String(i))}}function d(e,f){var g=\'/rest\'+ f;var h=e.split(\'&\').filter(function(i){var j=i.split(\'=\')[1];return j&&j.trim()!==\'\'}).join(\'&\');$.ajax({url:g,method:\'GET\',data:h,success:function(i){console.log(\'Settings saved successfully!\')},error:function(i,j,k){console.error(\'An error occurred while saving settings:\',k)}})}})</script>";


bool http_404_error(struct fs_file *file, int num_params, char *params[], char *values[]) {

    file->data = "{\"error\":404}";
    file->len = 13;
    file->index = 13;
    file->flags = FS_FILE_FLAGS_HEADER_INCLUDED;

    return true;
}


bool http_plot_weight(struct fs_file *file, int num_params, char *params[], char *values[]) {
    size_t len = strlen(html_plot_weight);

    file->data = html_plot_weight;
    file->len = len;
    file->index = len;
    file->flags = FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT;

    return true;
}

bool http_display_mirror(struct fs_file *file, int num_params, char *params[], char *values[]) {
    size_t len = strlen(html_display_mirror);

    file->data = html_display_mirror;
    file->len = len;
    file->index = len;
    file->flags = FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT;

    return true;
}

bool http_config(struct fs_file *file, int num_params, char *params[], char *values[]) {
    size_t len = strlen(html_config);

    file->data = html_config;
    file->len = len;
    file->index = len;
    file->flags = FS_FILE_FLAGS_HEADER_INCLUDED | FS_FILE_FLAGS_HEADER_PERSISTENT;

    return true;
}


bool rest_endpoints_init() {
    // rest_register_handler("/", http_rest_index);
    rest_register_handler("/404", http_404_error);
    rest_register_handler("/rest/scale_weight", http_rest_scale_weight);
    rest_register_handler("/rest/scale_config", http_rest_scale_config);
    rest_register_handler("/rest/charge_mode_config", http_rest_charge_mode_config);
    rest_register_handler("/rest/charge_mode_setpoint", http_rest_charge_mode_setpoint);
    rest_register_handler("/rest/system_control", http_rest_system_control);
    rest_register_handler("/rest/coarse_motor_config", http_rest_coarse_motor_config);
    rest_register_handler("/rest/fine_motor_config", http_rest_fine_motor_config);
    rest_register_handler("/rest/motor_speed", http_rest_motor_speed);
    rest_register_handler("/rest/button_control", http_rest_button_control);
    rest_register_handler("/rest/wireless_config", http_rest_wireless_config);
    rest_register_handler("/display_buffer", http_get_display_buffer);
    rest_register_handler("/display_mirror", http_display_mirror);
    rest_register_handler("/plot_weight", http_plot_weight);
    rest_register_handler("/config", http_config);
}