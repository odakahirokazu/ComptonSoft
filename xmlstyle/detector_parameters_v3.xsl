<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">
  <xsl:output method="xml" indent="yes" doctype-public="-//W3C//DTD XHTML 1.1//EN" doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"/>
  <xsl:template match="/">
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
        <title>Detector Parameters</title>
        <style type="text/css">
          html {background-color: #FFFFEE; color: #000000;}
          body {padding: 3%;}
          h1 {text-align: center;}
          table {margin:1em; border-collapse: collapse; border: 2px solid #000000; background-color: #FFFFDD}
          thead {border: 2px solid #000000; text-align: center;}
          tbody {border: 2px solid #000000; text-align: right;}
          th, td {border: 1px solid #000000; padding: 0.25em; }
          .detectors {border-top: solid 1px; border-left: solid 0.5em; padding: 0.375em; }
          .comment {color: #663300;}
          .col_efield {background-color: #EEEEFF;}
          .col_charge_collection {background-color: #EEEEFF;}
          .col_diffusion {background-color: #DDFFDD;}
          .col_channel_properties {background-color: #DDFFFF;}
          .common_param {background-color: #FFDDCC;}
          .special_param {color: #FF0000; }
        </style>
      </head>
      <body>
        <h1>Detector Parameters</h1>
        <p><xsl:text>Detector Name: </xsl:text><xsl:value-of select="detector_parameters/name" /></p>
        <xsl:apply-templates select="detector_parameters/comment" />
        <xsl:apply-templates select="detector_parameters/root_file" />
        <xsl:apply-templates select="detector_parameters/auto_position" />
        <xsl:apply-templates select="detector_parameters/sensitive_detector_not_found_warning" />
        <xsl:apply-templates select="detector_parameters/data/sensitive_detector" />
        <xsl:apply-templates select="detector_parameters/data/detector_set" />
      </body>
    </html>
  </xsl:template>
  <xsl:template match="comment">
    <p>Comment: <span class="comment"><xsl:value-of select="." /></span></p>
  </xsl:template>
  <xsl:template match="root_file">
    <p>ROOT file: <xsl:value-of select="." /> [<xsl:value-of select="@name" />]</p>
  </xsl:template>
  <xsl:template match="auto_position">
    <xsl:choose>
      <xsl:when test="@flag=1">
        <p>Auto Position Mode: ON</p>
      </xsl:when>
      <xsl:otherwise>
        <p>Auto Position Mode: OFF</p>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template match="sensitive_detector_not_found_warning">
    <xsl:choose>
      <xsl:when test="@flag=1">
        <p>Sensitive detector not found warning: ON</p>
      </xsl:when>
      <xsl:otherwise>
        <p>Sensitive detector not found warning: OFF</p>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template match="sensitive_detector">
    <h2 class="detectors"><xsl:value-of select="@name" /> (sensitive detector)</h2>
    <p class="setting"><xsl:text>Type: </xsl:text><xsl:value-of select="@type" /></p>
    <p class="setting"><xsl:text>Identification method: </xsl:text><xsl:value-of select="detector_list/@id_method" /></p>
    <p class="setting"><xsl:text>Layer offset: </xsl:text><xsl:value-of select="detector_list/@layer_offset" /></p>
    <xsl:call-template name="detectors" />
  </xsl:template>
  <xsl:template match="detector_set">
    <h2 class="detectors"><xsl:value-of select="@prefix" /> (prefix)</h2>
    <p class="setting"><xsl:text>Type: </xsl:text><xsl:value-of select="@type" /></p>
    <xsl:call-template name="detectors" />
  </xsl:template>
  <xsl:template name="detectors">
    <xsl:variable name="detector_type" select="@type" />
    <table>
      <colgroup class="col_detector_id">
        <col />
        <col />
        <col />
      </colgroup>
      <colgroup class="col_upside">
        <col />
      </colgroup>
      <colgroup class="col_depth">
        <col />
        <col />
      </colgroup>
      <colgroup class="col_conditions">
        <col />
        <col />
      </colgroup>
      <colgroup class="col_efield">
        <col />
        <col />
        <col />
        <col />
        <col />
        <col />
        <col />
        <col />
      </colgroup>
      <colgroup class="col_charge_collection">
        <col />
        <col />
        <col />
        <col />
        <col />
        <col />
        <col />
        <col />
      </colgroup>
      <colgroup class="col_diffusion">
        <col />
        <col />
        <col />
      </colgroup>
      <colgroup class="col_readout">
        <col />
        <col />
        <col />
      </colgroup>
      <xsl:choose>
        <xsl:when test="$detector_type='2DStrip'">
          <colgroup class="col_channel_properties">
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
          </colgroup>
          <colgroup class="col_channel_properties">
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
          </colgroup>
        </xsl:when>
        <xsl:otherwise>
          <colgroup class="col_channel_properties">
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
            <col />
          </colgroup>
        </xsl:otherwise>
      </xsl:choose>
      <colgroup class="col_reconstruction">
        <col />
        <xsl:if test="$detector_type='2DStrip'">
          <col />
          <col />
          <col />
          <col />
        </xsl:if>
      </colgroup>
      <tr>
        <th colspan="3"></th>
        <th></th>
        <th colspan="20">Sensor device simulation</th>
        <xsl:choose>
          <xsl:when test="$detector_type='2DStrip'">
            <th colspan="13">Channel properties (cathode)</th>
            <th colspan="13">Channel properties (anode)</th>
          </xsl:when>
          <xsl:otherwise>
            <th colspan="13">Channel properties</th>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="$detector_type='2DStrip'">
            <th colspan="5">Reconstruction</th>
          </xsl:when>
          <xsl:otherwise>
            <th colspan="1">Reconstruction</th>
          </xsl:otherwise>
        </xsl:choose>
      </tr>
      <tr>
        <th colspan="3">Detector ID</th>
        <th>Upside</th>
        <th colspan="2">Depth sensing</th>
        <th>Quenching</th>
        <th>Temperature</th>
        <th colspan="8">E-field</th>
        <th colspan="8">Charge collection</th>
        <th colspan="3">Diffusion</th>
        <th colspan="2">Timing resolution</th>
        <th>Pedestal gen.</th>
        <xsl:choose>
          <xsl:when test="$detector_type='2DStrip'">
            <th>Disable</th>
            <th colspan="2">Trigger discrimination</th>
            <th colspan="3">Noise level</th>
            <th>Compensation</th>
            <th>Pedestal</th>
            <th colspan="4">Gain correction</th>
            <th>Threshold</th>
            <th>Disable</th>
            <th colspan="2">Trigger discrimination</th>
            <th colspan="3">Noise level</th>
            <th>Compensation</th>
            <th>Pedestal</th>
            <th colspan="4">Gain correction</th>
            <th>Threshold</th>
          </xsl:when>
          <xsl:otherwise>
            <th>Disable</th>
            <th colspan="2">Trigger discrimination</th>
            <th colspan="3">Noise level</th>
            <th>Compensation</th>
            <th>Pedestal</th>
            <th colspan="4">Gain correction</th>
            <th>Threshold</th>
          </xsl:otherwise>
        </xsl:choose>
        <th>Reconstruction</th>
        <xsl:if test="$detector_type='2DStrip'">
          <th colspan="4">Energy consistency</th>
        </xsl:if>
      </tr>
      <tr>
        <th>ID</th>
        <th>Path</th>
        <th>Copy No.</th>
        <th></th>
        <th>mode</th>
        <th>resolution</th>
        <th>factor</th>
        <th></th>
        <th>Bias [V]</th>
        <th>mode</th>
        <th>P0</th>
        <th>P1</th>
        <th>P2</th>
        <th>P3</th>
        <th>WP map</th>
        <th>EField map</th>
        <th>mode</th>
        <th>CCE map</th>
        <th>MuTau e [cm2/V]</th>
        <th>MuTau h [cm2/V]</th>
        <th>Mobility e [cm2/V/s]</th>
        <th>Mobility h [cm2/V/s]</th>
        <th>Lifetime e [s]</th>
        <th>Lifetime h [s]</th>
        <th>mode</th>
        <th>anode</th>
        <th>cathode</th>
        <th>trigger</th>
        <th>energy measurement</th>
        <th>flag</th>
        <xsl:choose>
          <xsl:when test="$detector_type='2DStrip'">
            <th>status</th>
            <th>center</th>
            <th>sigma</th>
            <th>param0</th>
            <th>param1</th>
            <th>param2</th>
            <th>factor</th>
            <th>value</th>
            <th>status</th>
            <th>center</th>
            <th>sigma</th>
            <th>param0</th>
            <th>param1</th>
            <th>param2</th>
            <th>factor</th>
            <th>value</th>
            <th>c0</th>
            <th>c1</th>
            <th>c2</th>
            <th>c3</th>
            <th>value</th>
          </xsl:when>
          <xsl:otherwise>
            <th>status</th>
            <th>center</th>
            <th>sigma</th>
            <th>param0</th>
            <th>param1</th>
            <th>param2</th>
            <th>factor</th>
            <th>value</th>
            <th>c0</th>
            <th>c1</th>
            <th>c2</th>
            <th>c3</th>
            <th>value</th>
          </xsl:otherwise>
        </xsl:choose>
        <th>mode</th>
        <xsl:if test="$detector_type='2DStrip'">
          <th>lower C0</th>
          <th>lower C1</th>
          <th>upper C0</th>
          <th>upper C0</th>
        </xsl:if>
      </tr>
      <xsl:variable name="dummy" select="common" />
      <xsl:call-template name="make_row">
        <xsl:with-param name="detector_type" select="$detector_type" />
        <xsl:with-param name="common_parameters" select="common/parameters" />
        <xsl:with-param name="tr_class" select="'common_param'" />
        <xsl:with-param name="id" select="'COM'" />
        <xsl:with-param name="parameters" select="$dummy" />
      </xsl:call-template>
      <xsl:apply-templates select="detector_list/detector">
        <xsl:with-param name="detector_type" select="$detector_type" />
        <xsl:with-param name="common_parameters" select="common/parameters" />
      </xsl:apply-templates>
    </table>
  </xsl:template>
  <xsl:template match="detector">
    <xsl:param name="detector_type" select="''" />
    <xsl:param name="common_parameters" select="''" />
    <xsl:variable name="parameters" select="parameters" />
    <xsl:call-template name="make_row">
      <xsl:with-param name="detector_type" select="$detector_type" />
      <xsl:with-param name="common_parameters" select="$common_parameters" />
      <xsl:with-param name="id" select="@id" />
      <xsl:with-param name="path" select="@path" />
      <xsl:with-param name="copyno" select="@copyno" />
      <xsl:with-param name="parameters" select="parameters" />
    </xsl:call-template>
  </xsl:template>
  <xsl:template name="make_row">
    <xsl:param name="detector_type" select="''" />
    <xsl:param name="common_parameters" select="''" />
    <xsl:param name="tr_class" select="''" />
    <xsl:param name="id" select="''" />
    <xsl:param name="path" select="''" />
    <xsl:param name="copyno" select="''" />
    <xsl:param name="parameters" select="''" />
    <tr>
      <xsl:if test="string-length($tr_class)!=0">
        <xsl:attribute name="class"><xsl:value-of select="$tr_class" /></xsl:attribute>
      </xsl:if>
      <th><xsl:value-of select="$id" /></th>
      <td><xsl:value-of select="$path" /></td>
      <td><xsl:value-of select="$copyno" /></td>
      <xsl:variable name="final_upside_anode">
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/upside/@anode" />
          <xsl:with-param name="special" select="$parameters/upside/@anode" />
        </xsl:call-template>
      </xsl:variable>
      <xsl:variable name="final_upside_pixel">
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/upside/@pixel" />
          <xsl:with-param name="special" select="$parameters/upside/@pixel" />
        </xsl:call-template>
      </xsl:variable>
      <xsl:variable name="final_upside_xstrip">
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/upside/@xstrip" />
          <xsl:with-param name="special" select="$parameters/upside/@xstrip" />
        </xsl:call-template>
      </xsl:variable>
      <td>
        <span>
          <xsl:if test="string-length($parameters/upside/@anode)&gt;0 or string-length($parameters/upside/@pixel)&gt;0 or string-length($parameters/upside/@xstrip)&gt;0">
            <xsl:attribute name="class">special_param</xsl:attribute>
          </xsl:if>
          <xsl:call-template name="display_upside">
            <xsl:with-param name="upside_anode" select="$final_upside_anode" />
            <xsl:with-param name="upside_pixel" select="$final_upside_pixel" />
            <xsl:with-param name="upside_xstrip" select="$final_upside_xstrip" />
          </xsl:call-template>
        </span>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/depth_sensing/@mode" />
          <xsl:with-param name="special" select="$parameters/depth_sensing/@mode" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/depth_sensing/@resolution" />
          <xsl:with-param name="special" select="$parameters/depth_sensing/@resolution" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/quenching/@factor" />
          <xsl:with-param name="special" select="$parameters/quenching/@factor" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/temperature/@value" />
          <xsl:with-param name="special" select="$parameters/temperature/@value" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/efield/@bias" />
          <xsl:with-param name="special" select="$parameters/efield/@bias" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/efield/@mode" />
          <xsl:with-param name="special" select="$parameters/efield/@mode" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/efield/@param0" />
          <xsl:with-param name="special" select="$parameters/efield/@param0" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/efield/@param1" />
          <xsl:with-param name="special" select="$parameters/efield/@param1" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/efield/@param2" />
          <xsl:with-param name="special" select="$parameters/efield/@param2" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/efield/@param3" />
          <xsl:with-param name="special" select="$parameters/efield/@param3" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/efield/@wp_map_name" />
          <xsl:with-param name="special" select="$parameters/efield/@wp_map_name" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/efield/@efield_map_name" />
          <xsl:with-param name="special" select="$parameters/efield/@efield_map_name" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/charge_collection/@mode" />
          <xsl:with-param name="special" select="$parameters/charge_collection/@mode" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/charge_collection/@cce_map_name" />
          <xsl:with-param name="special" select="$parameters/charge_collection/@cce_map_name" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/charge_collection/mutau/@electron" />
          <xsl:with-param name="special" select="$parameters/charge_collection/mutau/@electron" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/charge_collection/mutau/@hole" />
          <xsl:with-param name="special" select="$parameters/charge_collection/mutau/@hole" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/charge_collection/mobility/@electron" />
          <xsl:with-param name="special" select="$parameters/charge_collection/mobility/@electron" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/charge_collection/mobility/@hole" />
          <xsl:with-param name="special" select="$parameters/charge_collection/mobility/@hole" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/charge_collection/lifetime/@electron" />
          <xsl:with-param name="special" select="$parameters/charge_collection/lifetime/@electron" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/charge_collection/lifetime/@hole" />
          <xsl:with-param name="special" select="$parameters/charge_collection/lifetime/@hole" />
        </xsl:call-template>
      </td>
      <xsl:variable name="final_diffusion_mode">
        <xsl:call-template name="display_parameter">
          <xsl:with-param name="common" select="$common_parameters/diffusion/@mode" />
          <xsl:with-param name="special" select="$parameters/diffusion/@mode" />
        </xsl:call-template>
      </xsl:variable>
      <xsl:choose>
        <xsl:when test="$final_diffusion_mode=0">
          <td>
            <span>
              <xsl:if test="string-length($parameters/diffusion/@mode)&gt;0">
                <xsl:attribute name="class">special_param</xsl:attribute>
              </xsl:if>
              <xsl:text>0: none</xsl:text>
            </span>
          </td>
          <td />
          <td />
        </xsl:when>
        <xsl:when test="$final_diffusion_mode=1">
          <td>
            <span>
              <xsl:if test="string-length($parameters/diffusion/@mode)&gt;0">
                <xsl:attribute name="class">special_param</xsl:attribute>
              </xsl:if>
              <xsl:text>1: factor</xsl:text>
            </span>
          </td>
          <td>
            <xsl:call-template name="display_parameter">
              <xsl:with-param name="common" select="$common_parameters/diffusion/spread_factor/@anode" />
              <xsl:with-param name="special" select="$parameters/diffusion/spread_factor/@anode" />
            </xsl:call-template>
          </td>
          <td>
            <xsl:call-template name="display_parameter">
              <xsl:with-param name="common" select="$common_parameters/diffusion/spread_factor/@cathode" />
              <xsl:with-param name="special" select="$parameters/diffusion/spread_factor/@cathode" />
            </xsl:call-template>
          </td>
        </xsl:when>
        <xsl:when test="$final_diffusion_mode=2">
          <td>
            <span>
              <xsl:if test="string-length($parameters/diffusion/@mode)&gt;0">
                <xsl:attribute name="class">special_param</xsl:attribute>
              </xsl:if>
              <xsl:text>2: constant</xsl:text>
            </span>
          </td>
          <td>
            <xsl:call-template name="display_parameter">
              <xsl:with-param name="common" select="$common_parameters/diffusion/constant/@anode" />
              <xsl:with-param name="special" select="$parameters/diffusion/constant/@anode" />
            </xsl:call-template>
            <xsl:text> cm</xsl:text>
          </td>
          <td>
            <xsl:call-template name="display_parameter">
              <xsl:with-param name="common" select="$common_parameters/diffusion/constant/@cathode" />
              <xsl:with-param name="special" select="$parameters/diffusion/constant/@cathode" />
            </xsl:call-template>
            <xsl:text> cm</xsl:text>
          </td>
        </xsl:when>
        <xsl:otherwise>
          <td>
            <span>
              <xsl:if test="string-length($parameters/diffusion/@mode)&gt;0">
                <xsl:attribute name="class">special_param</xsl:attribute>
              </xsl:if>
              <xsl:value-of select="$final_diffusion_mode" />
              <xsl:if test="string-length($final_diffusion_mode)&gt;0">
                <xsl:text>: unknown</xsl:text>
              </xsl:if>  
            </span>
          </td>
          <td />
          <td />
        </xsl:otherwise>
      </xsl:choose>
      <td>
        <xsl:call-template name="display_parameter" >
          <xsl:with-param name="common" select="$common_parameters/timing_resolution/@trigger" />
          <xsl:with-param name="special" select="$parameters/timing_resolution/@trigger" />
        </xsl:call-template>
      </td> 
      <td>
        <xsl:call-template name="display_parameter" >
          <xsl:with-param name="common" select="$common_parameters/timing_resolution/@energy_measurement" />
          <xsl:with-param name="special" select="$parameters/timing_resolution/@energy_measurement" />
        </xsl:call-template>
      </td>
      <td>
        <xsl:call-template name="display_parameter" >
          <xsl:with-param name="common" select="$common_parameters/pedestal_generation/@flag" />
          <xsl:with-param name="special" select="$parameters/pedestal_generation/@flag" />
        </xsl:call-template>
      </td>
      <xsl:choose>
        <xsl:when test="$detector_type='2DStrip'">
          <xsl:call-template name="display_channel_properties">
            <xsl:with-param name="common_parameters" select="$common_parameters/channel_properties[@side='cathode']" />
            <xsl:with-param name="parameters" select="$parameters/channel_properties[@side='cathode']" />
          </xsl:call-template>
          <xsl:call-template name="display_channel_properties">
            <xsl:with-param name="common_parameters" select="$common_parameters/channel_properties[@side='anode']" />
            <xsl:with-param name="parameters" select="$parameters/channel_properties[@side='anode']" />
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="display_channel_properties">
            <xsl:with-param name="common_parameters" select="$common_parameters/channel_properties" />
            <xsl:with-param name="parameters" select="$parameters/channel_properties" />
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
      <td>
        <xsl:call-template name="display_parameter" >
          <xsl:with-param name="common" select="$common_parameters/reconstruction/@mode" />
          <xsl:with-param name="special" select="$parameters/reconstruction/@mode" />
        </xsl:call-template>
      </td>
      <xsl:if test="$detector_type='2DStrip'">
        <td>
          <xsl:call-template name="display_parameter" >
            <xsl:with-param name="common" select="$common_parameters/reconstruction/energy_consistency_check/@lower_function_c0" />
            <xsl:with-param name="special" select="$parameters/reconstruction/energy_consistency_check/@lower_function_c0" />
          </xsl:call-template>
        </td>
        <td>
          <xsl:call-template name="display_parameter" >
            <xsl:with-param name="common" select="$common_parameters/reconstruction/energy_consistency_check/@lower_function_c1" />
            <xsl:with-param name="special" select="$parameters/reconstruction/energy_consistency_check/@lower_function_c1" />
          </xsl:call-template>
        </td>
        <td>
          <xsl:call-template name="display_parameter" >
            <xsl:with-param name="common" select="$common_parameters/reconstruction/energy_consistency_check/@upper_function_c0" />
            <xsl:with-param name="special" select="$parameters/reconstruction/energy_consistency_check/@upper_function_c0" />
          </xsl:call-template>
        </td>
        <td>
          <xsl:call-template name="display_parameter" >
            <xsl:with-param name="common" select="$common_parameters/reconstruction/energy_consistency_check/@upper_function_c1" />
            <xsl:with-param name="special" select="$parameters/reconstruction/energy_consistency_check/@upper_function_c1" />
          </xsl:call-template>
        </td>
      </xsl:if>
    </tr>
  </xsl:template>
  <xsl:template name="display_parameter">
    <xsl:param name="common" select="''" />
    <xsl:param name="special" select="''" />
    <xsl:choose>
      <xsl:when test="string-length($special)=0"><xsl:value-of select="$common" /></xsl:when>
      <xsl:otherwise><span class="special_param"><xsl:value-of select="$special" /></span></xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template name="display_upside">
    <xsl:param name="upside_anode" select="''" />
    <xsl:param name="upside_pixel" select="''" />
    <xsl:param name="upside_xstrip" select="''" />
    <xsl:if test="$upside_anode=1">anode</xsl:if>
    <xsl:if test="$upside_anode=0">cathode</xsl:if>
    <xsl:text>/</xsl:text>
    <xsl:if test="$upside_pixel=1">pixel</xsl:if>
    <xsl:if test="$upside_pixel=0">common</xsl:if>
    <xsl:if test="$upside_xstrip=1">x-strip</xsl:if>
    <xsl:if test="$upside_xstrip=0">y-strip</xsl:if>
  </xsl:template>
  <xsl:template name="display_channel_properties">
    <xsl:param name="common_parameters" />
    <xsl:param name="parameters" />
    <td>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/disable/@status" />
        <xsl:with-param name="special" select="$parameters/disable/@status" />
      </xsl:call-template>
    </td>
    <td>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/trigger_discrimination/@center" />
        <xsl:with-param name="special" select="$parameters/trigger_discrimination/@center" />
      </xsl:call-template>
    </td>
    <td>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/trigger_discrimination/@sigma" />
        <xsl:with-param name="special" select="$parameters/trigger_discrimination/@sigma" />
      </xsl:call-template>
    </td>
    <td>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/noise_level/@param0" />
        <xsl:with-param name="special" select="$parameters/noise_level/@param0" />
      </xsl:call-template>
    </td>
    <td>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/noise_level/@param1" />
        <xsl:with-param name="special" select="$parameters/noise_level/@param1" />
      </xsl:call-template>
    </td>
    <td>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/noise_level/@param2" />
        <xsl:with-param name="special" select="$parameters/noise_level/@param2" />
      </xsl:call-template>
    </td>
    <td>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/compensation/@factor" />
        <xsl:with-param name="special" select="$parameters/compensation/@factor" />
      </xsl:call-template>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/compensation/@function" />
        <xsl:with-param name="special" select="$parameters/compensation/@function" />
      </xsl:call-template>
    </td>
    <td>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/pedestal/@value" />
        <xsl:with-param name="special" select="$parameters/pedestal/@value" />
      </xsl:call-template>
    </td>
    <td>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/gain_correction/@c0" />
        <xsl:with-param name="special" select="$parameters/gain_correction/@c0" />
      </xsl:call-template>
    </td>
    <td>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/gain_correction/@c1" />
        <xsl:with-param name="special" select="$parameters/gain_correction/@c1" />
      </xsl:call-template>
    </td>
    <td>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/gain_correction/@c2" />
        <xsl:with-param name="special" select="$parameters/gain_correction/@c2" />
      </xsl:call-template>
    </td>
    <td>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/gain_correction/@c3" />
        <xsl:with-param name="special" select="$parameters/gain_correction/@c3" />
      </xsl:call-template>
    </td>
    <td>
      <xsl:call-template name="display_parameter" >
        <xsl:with-param name="common" select="$common_parameters/threshold/@value" />
        <xsl:with-param name="special" select="$parameters/threshold/@value" />
      </xsl:call-template>
    </td>
  </xsl:template>
</xsl:stylesheet>
