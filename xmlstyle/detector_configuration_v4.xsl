<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">
  <xsl:output method="html" version="5.0" indent="yes" />
  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
        <title>Detector Configuration</title>
        <style type="text/css">
          html {background-color: #FFFFDD; color: #000000;}
          body {padding: 1.5%;}
          h1 {text-align: center;}
          table {border-collapse: collapse; border: 2px solid #000000;}
          thead {border: 2px solid #000000; text-align: center;}
          tbody {border: 2px solid #000000; text-align: right;}
          th, td {border: 1px solid #000000; padding: 0.25em; }
          .comment {color: #663300;}
          .total-num {color: #009933;}
          table#physical-config tr td:nth-of-type(1) { text-align: left; }
          table#physical-config tr td:nth-of-type(2) { text-align: left; }
          table#physical-config tr td:nth-of-type(21) { text-align: left; }
          table#physical-config tr td:nth-of-type(23) { text-align: left; }
          table#readout-config tr td:nth-of-type(3) { text-align: left; }
          table#detector-groups tbody { text-align: left }
        </style>
      </head>
      <body>
        <h1>Detector Configuration</h1>
        <xsl:apply-templates />
      </body>
    </html>
  </xsl:template>
  <xsl:template match="name">
    <h2>Instrument: <xsl:value-of select="." /></h2>
  </xsl:template>
  <xsl:template match="length_unit">
    <p>Length unit: <xsl:value-of select="." /></p>
  </xsl:template>
  <xsl:template match="comment">
    <p>Comment: <span class="comment"><xsl:value-of select="." /></span></p>
  </xsl:template>
  <xsl:template match="detectors">
    <h2>Physical Configuration</h2>
    <p>Number of detectors = <span class="total-num"><xsl:value-of select="count(detector)" /></span></p>
    <table id="physical-config">
      <thead>
        <tr>
          <th>ID</th>
          <th>Name</th>
          <th>Type</th>
          <th colspan="3">Size</th>
          <th colspan="2">Offset</th>
          <th colspan="2">Num Pixel</th>
	        <th colspan="2">Pixel Pitch</th>
          <th colspan="3">Position</th>
          <th colspan="3">X-axis Direction</th>
          <th colspan="3">Y-axis Direction</th>
          <th>Energy Priority</th>
          <th colspan="2">Sections</th>
        </tr>
        <tr>
          <th></th>
          <th></th>
          <th></th>
          <th>X</th>
          <th>Y</th>
          <th>Z</th>
          <th>X</th>
          <th>Y</th>
          <th>X</th>
          <th>Y</th>
          <th>X</th>
          <th>Y</th>
          <th>X</th>
          <th>Y</th>
          <th>Z</th>
          <th>X</th>
          <th>Y</th>
          <th>Z</th>
          <th>X</th>
          <th>Y</th>
          <th>Z</th>
          <th></th>
          <th>Num</th>
          <th>section:(num_channels, electrode)</th>
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
      <td><xsl:value-of select="./@name" /></td>
      <td><xsl:value-of select="./@type" /></td>
      <td><xsl:value-of select="geometry/@x" /></td>
      <td><xsl:value-of select="geometry/@y" /></td>
      <td><xsl:value-of select="geometry/@z" /></td>
      <td><xsl:value-of select="offset/@x" /></td>
      <td><xsl:value-of select="offset/@y" /></td>
      <td><xsl:value-of select="pixel/@number_x" /></td>
      <td><xsl:value-of select="pixel/@number_y" /></td>
      <td><xsl:value-of select="pixel/@size_x" /></td>
      <td><xsl:value-of select="pixel/@size_y" /></td>
      <td><xsl:value-of select="position/@x" /></td>
      <td><xsl:value-of select="position/@y" /></td>
      <td><xsl:value-of select="position/@z" /></td>
      <td><xsl:value-of select="xaxis_direction/@x" /></td>
      <td><xsl:value-of select="xaxis_direction/@y" /></td>
      <td><xsl:value-of select="xaxis_direction/@z" /></td>
      <td><xsl:value-of select="yaxis_direction/@x" /></td>
      <td><xsl:value-of select="yaxis_direction/@y" /></td>
      <td><xsl:value-of select="yaxis_direction/@z" /></td>
      <td><xsl:value-of select="energy_priority/@electrode_side" /></td>
      <td><span class="total-num"><xsl:value-of select="count(sections/section)" /></span></td>
      <td><xsl:apply-templates select="sections/section" /></td>
    </tr>
  </xsl:template>
  <xsl:template match="sections/section">
    <span><xsl:number value="position()-1"/>:(<xsl:value-of select="./@num_channels" />, <xsl:value-of select="./@electrode_side" />) </span>
  </xsl:template>
  <xsl:template match="readout">
    <h2>Readout Configuration</h2>
    <p>Number of readout modules = <span class="total-num"><xsl:value-of select="count(module)" /></span></p>
    <table id="readout-config">
      <thead>
        <tr>
          <th>ID</th>
          <th>Length</th>
          <th>Configuration (Detector ID, Section Index)</th>
        </tr>
      </thead>
      <tbody>
        <xsl:apply-templates select="module" />
      </tbody>
    </table>
  </xsl:template>
  <xsl:template match="module">
    <tr>
      <td><xsl:value-of select="./@id" /></td>
      <td><span class="total-num"><xsl:value-of select="count(section)" /></span></td>
      <td><xsl:apply-templates select="./section" /></td>
    </tr>
  </xsl:template>
  <xsl:template match="module/section">
    <span>(<xsl:value-of select="./@detector_id" />, <xsl:value-of select="./@section" />) </span>
  </xsl:template>
  <xsl:template match="groups">
    <h2>Detector Groups and Hit Patterns</h2>
    <table id="detector-groups">
      <thead>
        <tr>
          <th>Type</th>
          <th>Name</th>
          <th>Short name</th>
          <th>Bit</th>
          <th>Definition (IDs/groups)</th>
        </tr>
      </thead>
      <tbody>
        <xsl:apply-templates select="group" />
        <xsl:apply-templates select="hit_pattern" />
      </tbody>
    </table>
  </xsl:template>
  <xsl:template match="group">
    <tr>
      <td>Group</td>
      <td><xsl:value-of select="./@name" /></td>
      <td></td>
      <td></td>
      <td><xsl:apply-templates select="./detector" /></td>
    </tr>
    <xsl:variable name="period" select="./@period" />
  </xsl:template>
  <xsl:template match="group/detector">
    <span><xsl:value-of select="./@id" /><xsl:if test="position()!=last()">, </xsl:if></span>
  </xsl:template>
  <xsl:template match="hit_pattern">
    <tr>
      <td>Hit Pattern</td>
      <td><xsl:value-of select="./@name" /></td>
      <td><xsl:value-of select="./@short_name" /></td>
      <td><xsl:value-of select="./@bit" /></td>
      <td><xsl:apply-templates select="./group" /></td>
    </tr>
  </xsl:template>
  <xsl:template match="hit_pattern/group">
    <span><xsl:value-of select="./@name" /><xsl:if test="position()!=last()">, </xsl:if></span>
  </xsl:template>
</xsl:stylesheet>
