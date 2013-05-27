<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">

<xsl:output method="xml" indent="yes" doctype-public="-//W3C//DTD XHTML 1.1//EN"  doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"/>

<xsl:template match="/">
  <html xmlns="http://www.w3.org/1999/xhtml">
    <head>
      <title>Data</title>
	  <style type="text/css">
            html {background-color: #FFDDFF; color: #000000;}
	    body {padding: 3%;}
	    h1 {text-align: center;}
	    table {margin:1em; border-collapse: collapse; border: 2px solid #000000;}
	    thead {border: 2px solid #000000; text-align: center;}
	    tbody {border: 2px solid #000000; text-align: right;}
	    th, td {border: 1px solid #000000;}
	  </style>
    </head>

    <body>
      <h1>Detector Channel Map</h1>
      <xsl:apply-templates />
    </body>
  </html>
</xsl:template>

<xsl:template match="channel_map/detector">
  <h2>Detector: <xsl:value-of select="./@name" /></h2>
  <p>total chip: <xsl:value-of select="./@total" /></p>
  <xsl:apply-templates select="chip" />
  <hr />
</xsl:template>

<xsl:template match="chip">
  <h3>Chip ID: <xsl:value-of select="./@id" /> (<xsl:value-of select="./@total" /> channels)</h3>
  <table>
    <thead>
      <tr>
        <th>ID</th>
        <th>Strip X</th>
	<th>Strip Y</th>
      </tr>
    </thead>
    <tbody>
      <xsl:apply-templates select="channel" />
    </tbody>
  </table>
</xsl:template>

<xsl:template match="channel">
  <tr>
    <th><xsl:value-of select="./@id" /></th>
    <td><xsl:value-of select="x" /></td>
    <td><xsl:value-of select="y" /></td>
  </tr>
</xsl:template>

</xsl:stylesheet>
