<%inherit file="base.tpl"/>

<h1>Pollux nz city configuration utility</h1>
%if welldone:
<br />
<div class="alert alert-success">Configuration successfully updated<a class="close" data-dismiss="alert" href="#">&times;</a></div>
%else:
<div id="osm_err" class="alert alert-error hide fade in" data-alert="alert"><a class="close" data-dismiss="alert" href="#">&times;</a><span id="osm_err_t"></span></div>
%endif
<form class="form-horizontal" method="post">

<fieldset>
   <legend>Sensor List</legend>
%for name, sensorl in sensors.iteritems():
<h3>Sensor</h3><br />
<div class="control-group">
   <label class="control-label" for="sensor_addr">Sensor's address</label>
   <div class="controls">
    <input type="text" class="input-medium" id="sensor_addr" name="sensor_addr" value="${name}">
    <input type="hidden" id="sensor_addr_old" name="sensor_addr_old" value="${name}">
    <!--<p class="help-block">Supporting help text</p>-->
   </div>
</div>

<table class="table table-striped table-bordered">
   	<thead>
   		<tr>
   			<th width="80px">Activated</th>
   			<th>Sensor</th>
   		</tr>
   	</thead>
	<tbody>
%for s in sensorl:
		<tr>
<%
checked = "checked" if s["activated"] else "" 
%>
			<td><input type="checkbox" name="${s["address"]}_${s["register"]}" id="${s["address"]}_${s["register"]}" ${checked} /></td>
			<td><label for="${s["address"]}_${s["register"]}">${s["name"]}
%if "unit" in s.keys():
			(${s["unit"]})
%endif
			</label></td>
		</tr>
%endfor
   	</tbody>
   </table>

%endfor

</fieldset>
	<center><input type="submit" class="btn btn-primary btn-large"/></center>
</form>