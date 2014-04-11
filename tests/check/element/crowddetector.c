/*
 * (C) Copyright 2014 Kurento (http://kurento.org/)
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 */
#include <gst/check/gstcheck.h>
#include <gst/gst.h>
#include <glib.h>
#include "kmsuriendpointstate.h"

#include <kmstestutils.h>

#define ROIS_PARAM "rois"

GMainLoop *loop;

GstStructure *
get_roi_structure (const gchar * id)
{
  int pointCount = 0;
  GstStructure *roiStructure, *configRoiSt;

  roiStructure = gst_structure_new_empty (id);
  for (pointCount = 0; pointCount < 4; pointCount++) {
    GstStructure *pointSt;
    gchar *name;

    name = g_strdup_printf ("point%d", pointCount);
    pointSt = gst_structure_new (name,
        "x", G_TYPE_INT, 10 + pointCount,
        "y", G_TYPE_INT, 10 + pointCount, NULL);
    gst_structure_set (roiStructure, name, GST_TYPE_STRUCTURE, pointSt, NULL);
    gst_structure_free (pointSt);
    g_free (name);
  }
  configRoiSt = gst_structure_new ("config",
      "id", G_TYPE_STRING, id,
      "occupancy_level_min", G_TYPE_INT, 10,
      "occupancy_level_med", G_TYPE_INT, 35,
      "occupancy_level_max", G_TYPE_INT, 65,
      "occupancy_num_frames_to_event", G_TYPE_INT,
      5,
      "fluidity_level_min", G_TYPE_INT, 10,
      "fluidity_level_med", G_TYPE_INT, 35,
      "fluidity_level_max", G_TYPE_INT, 65,
      "fluidity_num_frames_to_event", G_TYPE_INT, 5,
      "send_optical_flow_event", G_TYPE_BOOLEAN, FALSE,
      "optical_flow_num_frames_to_event", G_TYPE_INT,
      3,
      "optical_flow_num_frames_to_reset", G_TYPE_INT,
      3, "optical_flow_angle_offset", G_TYPE_INT, 0, NULL);
  gst_structure_set (roiStructure, "config", GST_TYPE_STRUCTURE, configRoiSt,
      NULL);
  gst_structure_free (configRoiSt);
  return roiStructure;
}

GST_START_TEST (set_properties)
{
  GstElement *crowddetector;
  GstStructure *roisStructure;
  GstStructure *roiStructureAux;

  crowddetector = gst_element_factory_make ("crowddetector", NULL);

  roisStructure = gst_structure_new_empty ("Rois");
  roiStructureAux = get_roi_structure ("roi1");
  gst_structure_set (roisStructure,
      "roi1", GST_TYPE_STRUCTURE, roiStructureAux, NULL);
  gst_structure_free (roiStructureAux);

  roiStructureAux = get_roi_structure ("roi2");
  gst_structure_set (roisStructure,
      "roi2", GST_TYPE_STRUCTURE, roiStructureAux, NULL);
  gst_structure_free (roiStructureAux);

  g_object_set (G_OBJECT (crowddetector), ROIS_PARAM, roisStructure, NULL);

  g_object_set (G_OBJECT (crowddetector), ROIS_PARAM, roisStructure, NULL);

  g_object_set (G_OBJECT (crowddetector), ROIS_PARAM, roisStructure, NULL);

  gst_structure_free (roisStructure);

  g_object_unref (crowddetector);
}

GST_END_TEST static void
bus_msg (GstBus * bus, GstMessage * msg, gpointer pipe)
{
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_ERROR:{
      GST_ERROR ("Error: %" GST_PTR_FORMAT, msg);
      GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS (GST_BIN (pipe),
          GST_DEBUG_GRAPH_SHOW_ALL, "error");
      fail ("Error received on bus");
      break;
    }
    case GST_MESSAGE_EOS:{
      g_main_loop_quit (loop);
      break;
    }
    case GST_MESSAGE_WARNING:{
      GST_ERROR ("Warning: %" GST_PTR_FORMAT, msg);
      GST_DEBUG_BIN_TO_DOT_FILE_WITH_TS (GST_BIN (pipe),
          GST_DEBUG_GRAPH_SHOW_ALL, "error");
      fail ("Warning received on bus");
      break;
    }
    default:
      break;
  }
}

GST_START_TEST (player_with_filter)
{
  GstElement *player, *pipeline, *filter, *fakesink_audio, *fakesink_video;
  guint bus_watch_id;
  GstBus *bus;
  GstStructure *roisStructure;
  GstStructure *roiStructureAux;

  loop = g_main_loop_new (NULL, FALSE);
  pipeline = gst_pipeline_new ("pipeline_live_stream");
  g_object_set (G_OBJECT (pipeline), "async-handling", TRUE, NULL);
  player = gst_element_factory_make ("playerendpoint", NULL);
  filter = gst_element_factory_make ("crowddetector", NULL);
  fakesink_audio = gst_element_factory_make ("fakesink", NULL);
  fakesink_video = gst_element_factory_make ("fakesink", NULL);
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));

  bus_watch_id = gst_bus_add_watch (bus, gst_bus_async_signal_func, NULL);
  g_signal_connect (bus, "message", G_CALLBACK (bus_msg), pipeline);
  g_object_unref (bus);

  g_object_set (G_OBJECT (player), "uri",
      "http://ci.kurento.com/downloads/crowd.mp4", NULL);

  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  gst_bin_add (GST_BIN (pipeline), filter);
  gst_element_set_state (filter, GST_STATE_PLAYING);

  gst_bin_add (GST_BIN (pipeline), fakesink_audio);
  gst_element_set_state (fakesink_audio, GST_STATE_PLAYING);

  gst_bin_add (GST_BIN (pipeline), fakesink_video);
  gst_element_set_state (fakesink_video, GST_STATE_PLAYING);

  gst_bin_add (GST_BIN (pipeline), player);
  gst_element_set_state (player, GST_STATE_PLAYING);

  kms_element_link_pads (player, "audio_src_%u", fakesink_audio, "sink");
  kms_element_link_pads (player, "video_src_%u", filter, "sink");

  gst_element_link (filter, fakesink_video);

  roisStructure = gst_structure_new_empty ("Rois");
  roiStructureAux = get_roi_structure ("roi1");
  gst_structure_set (roisStructure,
      "roi1", GST_TYPE_STRUCTURE, roiStructureAux, NULL);
  gst_structure_free (roiStructureAux);
  g_object_set (G_OBJECT (filter), ROIS_PARAM, roisStructure, NULL);
  gst_structure_free (roisStructure);

  /* Set player to start state */
  g_object_set (G_OBJECT (player), "state", KMS_URI_ENDPOINT_STATE_START, NULL);

  g_main_loop_run (loop);

  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (pipeline));
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);
}

GST_END_TEST static Suite *
crowddetector_suite (void)
{
  Suite *s = suite_create ("crowddetector");
  TCase *tc_chain = tcase_create ("element");

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, set_properties);
  tcase_add_test (tc_chain, player_with_filter);

  return s;
}

GST_CHECK_MAIN (crowddetector);
