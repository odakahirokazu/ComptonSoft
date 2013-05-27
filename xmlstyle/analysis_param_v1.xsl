<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">
  <xsl:output method="xml" indent="yes" doctype-public="-//W3C//DTD XHTML 1.1//EN" doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"/>
  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
	<title>Analysis param</title>
	<style type="text/css">
	  html {background-color: #DDFFDD; color: #000000;}
	  body {padding: 3%;}
	  h1 {text-align: center;}
	  .comment {color: #663300;}
	  table {margin:1em; border-collapse: collapse; border: 2px solid #000000;}
	  thead {border: 2px solid #000000; text-align : center;}
	  tbody {border: 2px solid #000000; text-align: right;}
	  th, td {border: 1px solid #000000;}
	</style>
	<style type="text/css"/>
      </head>
      <body>
	<h1>Analysis Parameters</h1>
	<xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="analysis/name">
    <p>Detector Name: 
    <xsl:value-of select="."/>
    </p>
  </xsl:template>

  <xsl:template match="comment">
    <p>Comment: 
    <span class="comment">
      <xsl:value-of select="."/>
    </span>
    </p>
  </xsl:template>

  <xsl:template match="detectors">
    <h2>Detector Information</h2>
    <table>
      <thead>
	<tr>
	  <th>name</th>
	  <th>detector type</th>
	  <th>param00</th>
	  <th>param01</th>
	  <th>param02</th>
	  <th>param10</th>
	  <th>param11</th>
	  <th>param12</th>
	  <th>analysis mode</th>
	  <th>threshold (keV)</th>
	</tr>
      </thead>
      <tbody>
	<xsl:apply-templates select="detector"/>
      </tbody>
    </table>
    <br/>
    <h2>description</h2>
    <h3>detector type</h3>
    <p>1: Pad, 2: DSD, 3: Scintillator</p>
    <h3>Noise parameters</h3>
    <p>(deltaE)^2 = (param00)^2 + E*(param01)^2 + E^2*(param02)^2</p>
    <p>(deltaE)^2 = (param10)^2 + E*(param11)^2 + E^2*(param12)^2 for the other side</p>

  </xsl:template>

  <xsl:template match="detector">
    <tr>
      <td>
	<xsl:value-of select="name"/>
      </td>
      <td>
	<xsl:value-of select="type"/>
      </td>
      <td>
	<xsl:value-of select="noiselevel/param00"/>
      </td>
      <td>
	<xsl:value-of select="noiselevel/param01"/>
      </td>
      <td>
	<xsl:value-of select="noiselevel/param02"/>
      </td>
      <td>
	<xsl:value-of select="noiselevel/param10"/>
      </td>
      <td>
	<xsl:value-of select="noiselevel/param11"/>
      </td>
      <td>
	<xsl:value-of select="noiselevel/param12"/>
      </td>
      <td>
	<xsl:value-of select="analysis/mode"/>
      </td>
      <td>
	<xsl:value-of select="analysis/threshold"/>
      </td>
    </tr>
  </xsl:template>

</xsl:stylesheet>
