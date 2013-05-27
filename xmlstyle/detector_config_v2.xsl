<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">

<xsl:output method="xml" indent="yes" doctype-public="-//W3C//DTD XHTML 1.1//EN"  doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"/>

<xsl:template match="/">
  <html xmlns="http://www.w3.org/1999/xhtml">
    <head>
      <title>Data</title>
	  <style type="text/css">
            html {background-color: #FFFFDD; color: #000000;}
	    body {padding: 3%;}
	    h1 {text-align: center;}
	    .comment {color: #663300;}
	    .totalnum {color: #FF0000;}
	    table {margin:1em; border-collapse: collapse; border: 2px solid #000000;}
	    thead {border: 2px solid #000000; text-align: center;}
	    tbody {border: 2px solid #000000; text-align: right;}
	    th, td {border: 1px solid #000000;}
	  </style>
    </head>

    <body>
      <h1>Detector Configuration</h1>
      <xsl:apply-templates />
    </body>
  </html>
</xsl:template>

<xsl:template match="name">
  <p>Detector Name: <xsl:value-of select="." /></p>
</xsl:template>

<xsl:template match="comment">
  <p>COMMENT: <span class="comment"><xsl:value-of select="." /></span></p>
</xsl:template>

<xsl:template match="detectors">
  <h2>Detector Information</h2>
  <p>Total Detectors = <span class="totalnum"><xsl:value-of select="./@total" /></span></p>
  <table>
    <thead>
      <tr>
        <th>ID</th>
        <th>Name</th>
	<th>Type</th>
        <th>Width X</th>
        <th>Width Y</th>
	<th>Thckness</th>
	<th>Offset X</th>
	<th>Offset Y</th>
	<th>Num Strip X</th>
	<th>Num Strip Y</th>
	<th>Strip Pitch X</th>
	<th>Strip Pitch Y</th>
	<th>Position X</th>
	<th>Position Y</th>
	<th>Position Z</th>
	<th>X-axis Direction X</th>
	<th>X-axis Direction Y</th>
	<th>X-axis Direction Z</th>
	<th>Y-axis Direction X</th>
	<th>Y-axis Direction Y</th>
	<th>Y-axis Direction Z</th>
	<th>Energy Priority</th>
	<th>ASIC Num</th>
	<th>ASIC Data (id, channel, nside)</th>
      </tr>
    </thead>
    <tbody>
    <xsl:apply-templates select="detector" />
    </tbody>
  </table>
</xsl:template>

<xsl:template match="detector">
  <tr>
    <th><xsl:value-of select="./@id" /></th>
    <td><xsl:value-of select="name" /></td>
    <td><xsl:value-of select="type" /></td>
    <td><xsl:value-of select="geometry/widthx" /></td>
    <td><xsl:value-of select="geometry/widthy" /></td>
    <td><xsl:value-of select="geometry/thickness" /></td>
    <td><xsl:value-of select="geometry/offsetx" /></td>
    <td><xsl:value-of select="geometry/offsety" /></td>
    <td><xsl:value-of select="strip/numx" /></td>
    <td><xsl:value-of select="strip/numy" /></td>
    <td><xsl:value-of select="strip/pitchx" /></td>
    <td><xsl:value-of select="strip/pitchy" /></td>
    <td><xsl:value-of select="position/x" /></td>
    <td><xsl:value-of select="position/y" /></td>
    <td><xsl:value-of select="position/z" /></td>
    <td><xsl:value-of select="direction_xaxis/x" /></td>
    <td><xsl:value-of select="direction_xaxis/y" /></td>
    <td><xsl:value-of select="direction_xaxis/z" /></td>
    <td><xsl:value-of select="direction_yaxis/x" /></td>
    <td><xsl:value-of select="direction_yaxis/y" /></td>
    <td><xsl:value-of select="direction_yaxis/z" /></td>
    <td><xsl:value-of select="energy_priority" /></td>
    <td><span class="totalnum"><xsl:value-of select="asics/@total" /></span></td>
    <td><xsl:apply-templates select="asics/asic" /></td>
  </tr>
</xsl:template>

<xsl:template match="asic">
  <span>(<xsl:number value="position()-1"/>, <xsl:value-of select="./@channel" />, <xsl:value-of select="./@nside" />) </span>
</xsl:template>


<xsl:template match="readout">
  <h2>Readout Module</h2>
  <p>Total Readout Modules = <span class="totalnum"><xsl:value-of select="./@total" /></span></p>
  <p>Module ID [total chip]: (Detector ID - Chip ID)</p>
  <xsl:apply-templates select="module" />
</xsl:template>

<xsl:template match="module">
  <div>ID <xsl:value-of select="./@id" /> [<span class="totalnum"><xsl:value-of select="./@total" /></span>]: <xsl:apply-templates select="mod_asic" /></div>
</xsl:template>

<xsl:template match="mod_asic">
  <span>(<xsl:value-of select="detid" />-<xsl:apply-templates select="chipid" />) </span>
</xsl:template>

</xsl:stylesheet>
