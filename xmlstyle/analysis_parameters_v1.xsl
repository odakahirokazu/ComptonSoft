<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">
  <xsl:output method="xml" indent="yes" doctype-public="-//W3C//DTD XHTML 1.1//EN" doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"/>
  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
	<title>Analysis Parameters</title>
	<style type="text/css">
          html {background-color: #DDFFDD; color: #000000;}
          body {padding: 3%;}
          h1 {text-align: center;}
          .comment {color: #663300;}
          table {margin:1em; border-collapse: collapse; border: 2px solid #000000;}
          thead {border: 2px solid #000000; text-align : center;}
          tbody {border: 2px solid #000000; text-align: right;}
          th, td {border: 1px solid #000000;}
          tbody tr td:nth-of-type(1) { text-align: left; }
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
          <th>Prefix</th>
          <th>Detector Type</th>
          <th colspan="3">Noise parameters</th>
          <th colspan="3">Noise (cathode)</th>
          <th colspan="3">Noise (anode)</th>
          <th>Reconstruction mode</th>
          <th colspan="3">Threshold (keV)</th>
          <th colspan="4">E-check function</th>
	</tr>
	<tr>
          <th></th>
          <th></th>
          <th>0 (keV)</th>
          <th>1</th>
          <th>2</th>
          <th>0 (keV)</th>
          <th>1</th>
          <th>2</th>
          <th>0 (keV)</th>
          <th>1</th>
          <th>2</th>
          <th></th>
          <th>common</th>
          <th>cathode</th>
          <th>anode</th>
          <th>lower C0 (keV)</th>
          <th>lower C1</th>
          <th>upper C0 (keV)</th>
          <th>upper C1</th>
	</tr>
      </thead>
      <tbody>
	<xsl:apply-templates select="detector"/>
      </tbody>
    </table>
    <br/>
    <h2>Description</h2>
    <h3>Detector type</h3>
    <p>1: Pad, 2: DSD, 3: Scintillator</p>
    <h3>Noise parameters</h3>
    <p>(deltaE)^2 = (param0)^2 + (param1*sqrt(E))^2 + (param2*E)^2</p>
    <p>Energy: in units of keV.</p>
  </xsl:template>

  <xsl:template match="detector">
    <tr>
      <td><xsl:value-of select="prefix"/></td>
      <td><xsl:value-of select="type"/></td>
      <td><xsl:value-of select="noiselevel/param0"/></td>
      <td><xsl:value-of select="noiselevel/param1"/></td>
      <td><xsl:value-of select="noiselevel/param2"/></td>
      <td><xsl:value-of select="noiselevel/cathode_param0"/></td>
      <td><xsl:value-of select="noiselevel/cathode_param1"/></td>
      <td><xsl:value-of select="noiselevel/cathode_param2"/></td>
      <td><xsl:value-of select="noiselevel/anode_param0"/></td>
      <td><xsl:value-of select="noiselevel/anode_param1"/></td>
      <td><xsl:value-of select="noiselevel/anode_param2"/></td>
      <td><xsl:value-of select="reconstruction/mode"/></td>
      <td><xsl:value-of select="reconstruction/threshold"/></td>
      <td><xsl:value-of select="reconstruction/threshold_cathode"/></td>
      <td><xsl:value-of select="reconstruction/threshold_anode"/></td>
      <td><xsl:value-of select="reconstruction/energy_consistency_check/lower_function_c0"/></td>
      <td><xsl:value-of select="reconstruction/energy_consistency_check/lower_function_c1"/></td>
      <td><xsl:value-of select="reconstruction/energy_consistency_check/upper_function_c0"/></td>
      <td><xsl:value-of select="reconstruction/energy_consistency_check/upper_function_c1"/></td>
    </tr>
  </xsl:template>
</xsl:stylesheet>
