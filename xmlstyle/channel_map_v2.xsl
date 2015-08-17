<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">
  <xsl:output method="html" version="5.0" indent="yes" />
  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
        <title>Detector Channel Map</title>
	      <style type="text/css">
          html {background-color: #FFDDFF; color: #000000;}
	        body {padding: 1.5%;}
	        h1 {text-align: center;}
          .comment {color: #663300;}
	        .total-num {color: #009933;}
          table {border-collapse: collapse; border: 2px solid #000000;}
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
  <xsl:template match="name">
    <h2>Instrument: <xsl:value-of select="." /></h2>
  </xsl:template>
  <xsl:template match="comment">
    <p class="comment">Comment: <span class="comment"><xsl:value-of select="." /></span></p>
  </xsl:template>
  <xsl:template match="channel_map/detector">
    <h2>Detector: <xsl:value-of select="./@name" /></h2>
    <ul>
      <li>Type: <xsl:value-of select="./@type" /></li>
      <li><xsl:value-of select="./@num_sections" /> sections</li>
      <li><xsl:value-of select="./@num_channels" /> channels in each section</li>
      <li><xsl:value-of select="./@num_x" /> &#215; <xsl:value-of select="./@num_y" /> pixels (x, y)</li>
      <!--li>Number of sections: <span class="total-num"><xsl:value-of select="count(section)" /></span> (check)</li-->
    </ul>
    <xsl:apply-templates select="section" />
    <hr />
  </xsl:template>
  <xsl:template match="section">
    <h3>Section <xsl:value-of select="./@id" /></h3>
    <p>Number of channels: <span class="total-num"><xsl:value-of select="count(channel)" /></span></p>
    <table>
      <thead>
        <tr>
          <th>Channel</th>
          <th>Pixel X</th>
	        <th>Pixel Y</th>
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
