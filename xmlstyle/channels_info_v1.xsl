<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">
  <xsl:output method="xml" indent="yes" doctype-public="-//W3C//DTD XHTML 1.1//EN"  doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd" />
  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
        <title>Channels Information</title>
	      <style type="text/css">
          html {background-color: #DDDDEE; color: #000000;}
          body {padding: 3%;}
          h1 {text-align: center;}
          table {margin:1em; border-collapse: collapse; border: 2px solid #000000;}
          thead {border: 2px solid #000000; text-align: center;}
          tbody {border: 2px solid #000000; text-align: right;}
          th, td {border: 1px solid #000000; padding: 0.25em; }
          .common_param {background-color: #AAAAFF;}
          .special_param {color: #FF0000; }
	      </style>
      </head>
      <body>
        <h1>Channels Information</h1>
        <xsl:apply-templates />
      </body>
    </html>
  </xsl:template>
  <xsl:template match="channels_info">
    <p>Instrument: <xsl:value-of select="name" /></p>
    <p>Comment: <xsl:value-of select="comment" /></p>
    <xsl:apply-templates select="data" />
  </xsl:template>
  <xsl:template match="data">
    <table>
      <thead>
        <tr>
          <th colspan="4">Channel ID</th>
          <th>Disable</th>
          <th colspan="2">Trigger discrimination</th>
          <th colspan="3">Noise level</th>
          <th>Compensation</th>
          <th>Threshold</th>
        </tr>
        <tr>
          <th>base</th>
          <th>ID</th>
          <th>section</th>
          <th>channel</th>
          <th>status</th>
          <th>center</th>
          <th>sigma</th>
          <th>param0</th>
          <th>param1</th>
          <th>param2</th>
          <th>factor</th>
          <th>value</th>
        </tr>
      </thead>
      <tbody>
        <xsl:apply-templates />
      </tbody>
    </table>
  </xsl:template>
  <xsl:template match="detector">
    <xsl:variable name="id" select="./@id" />
    <xsl:apply-templates select="section">
      <xsl:with-param name="type" select="'Detector'" />
      <xsl:with-param name="id" select="$id" />
    </xsl:apply-templates>
  </xsl:template>
  <xsl:template match="readout_module">
    <xsl:variable name="id" select="./@id" />
    <xsl:apply-templates select="section">
      <xsl:with-param name="type" select="'Readout'" />
      <xsl:with-param name="id" select="$id" />
    </xsl:apply-templates>
  </xsl:template>
  <xsl:template match="section">
    <xsl:param name="type" select="''" />
    <xsl:param name="id" select="''" />
    <xsl:variable name="section" select="./@id" />
    <xsl:variable name="com_disable" select="common/disable/@status" />
    <xsl:variable name="com_trig_center" select="common/trigger_discrimination/@center" />
    <xsl:variable name="com_trig_sigma" select="common/trigger_discrimination/@sigma" />
    <xsl:variable name="com_noise_p0" select="common/noise_level/@param0" />
    <xsl:variable name="com_noise_p1" select="common/noise_level/@param1" />
    <xsl:variable name="com_noise_p2" select="common/noise_level/@param2" />
    <xsl:variable name="com_compensation" select="common/compensation/@factor" />
    <xsl:variable name="com_threshold" select="common/threshold/@value" />
    <tr class="common_param">
      <td><xsl:value-of select="$type" /></td>
      <td><xsl:value-of select="$id" /></td>
      <td><xsl:value-of select="$section" /></td>
      <td>
        <xsl:choose>
          <xsl:when test="@all=1">ALL</xsl:when>
          <xsl:otherwise>COM</xsl:otherwise>
        </xsl:choose>
      </td>
      <td><xsl:value-of select="$com_disable" /></td>
      <td><xsl:value-of select="$com_trig_center" /></td>
      <td><xsl:value-of select="$com_trig_sigma" /></td>
      <td><xsl:value-of select="$com_noise_p0" /></td>
      <td><xsl:value-of select="$com_noise_p1" /></td>
      <td><xsl:value-of select="$com_noise_p2" /></td>
      <td><xsl:value-of select="$com_compensation" /></td>
      <td><xsl:value-of select="$com_threshold" /></td>
    </tr>
    <xsl:apply-templates select="channel">
      <xsl:with-param name="type" select="$type" />
      <xsl:with-param name="id" select="$id" />
      <xsl:with-param name="section" select="$section" />
      <xsl:with-param name="com_disable" select="$com_disable" />
      <xsl:with-param name="com_trig_center" select="$com_trig_center" />
      <xsl:with-param name="com_trig_sigma" select="$com_trig_sigma" />
      <xsl:with-param name="com_noise_p0" select="$com_noise_p0" />
      <xsl:with-param name="com_noise_p1" select="$com_noise_p1" />
      <xsl:with-param name="com_noise_p2" select="$com_noise_p2" />
      <xsl:with-param name="com_compensation" select="$com_compensation" />
      <xsl:with-param name="com_threshold" select="$com_threshold" />
    </xsl:apply-templates>
  </xsl:template>
  <xsl:template match="channel">
    <xsl:param name="type" select="''" />
    <xsl:param name="id" select="''" />
    <xsl:param name="section" select="''" />
    <xsl:param name="com_disable" select="''" />
    <xsl:param name="com_trig_center" select="''" />
    <xsl:param name="com_trig_sigma" select="''" />
    <xsl:param name="com_noise_p0" select="''" />
    <xsl:param name="com_noise_p1" select="''" />
    <xsl:param name="com_noise_p2" select="''" />
    <xsl:param name="com_compensation" select="''" />
    <xsl:param name="com_threshold" select="''" />
    <xsl:variable name="channel" select="./@id" />
    <xsl:variable name="disable" select="disable/@status" />
    <xsl:variable name="trig_center" select="trigger_discrimination/@center" />
    <xsl:variable name="trig_sigma" select="trigger_discrimination/@sigma" />
    <xsl:variable name="noise_p0" select="noise_level/@param0" />
    <xsl:variable name="noise_p1" select="noise_level/@param1" />
    <xsl:variable name="noise_p2" select="noise_level/@param2" />
    <xsl:variable name="compensation" select="compensation/@factor" />
    <xsl:variable name="threshold" select="threshold/@value" />
    <tr>
      <td><xsl:value-of select="$type" /></td>
      <td><xsl:value-of select="$id" /></td>
      <td><xsl:value-of select="$section" /></td>
      <td><xsl:value-of select="$channel" /></td>
      <td>
        <xsl:call-template name="display_parameter" >
          <xsl:with-param name="common" select="$com_disable" />
          <xsl:with-param name="special" select="$disable" />
        </xsl:call-template>
      </td> 
      <td>
        <xsl:call-template name="display_parameter" >
          <xsl:with-param name="common" select="$com_trig_center" />
          <xsl:with-param name="special" select="$trig_center" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter" >
          <xsl:with-param name="common" select="$com_trig_sigma" />
          <xsl:with-param name="special" select="$trig_sigma" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter" >
          <xsl:with-param name="common" select="$com_noise_p0" />
          <xsl:with-param name="special" select="$noise_p0" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter" >
          <xsl:with-param name="common" select="$com_noise_p1" />
          <xsl:with-param name="special" select="$noise_p1" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter" >
          <xsl:with-param name="common" select="$com_noise_p2" />
          <xsl:with-param name="special" select="$noise_p2" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter" >
          <xsl:with-param name="common" select="$com_compensation" />
          <xsl:with-param name="special" select="$compensation" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter" >
          <xsl:with-param name="common" select="$com_threshold" />
          <xsl:with-param name="special" select="$threshold" />
        </xsl:call-template>
      </td>
    </tr>
  </xsl:template>
  <xsl:template name="display_parameter">
    <xsl:param name="common" select="''" />
    <xsl:param name="special" select="''" />
    <xsl:choose>
      <xsl:when test="string-length($special)=0">
        <xsl:value-of select="$common" />
      </xsl:when>
      <xsl:otherwise>
        <span class="special_param">
          <xsl:value-of select="$special" />
        </span>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
</xsl:stylesheet>
