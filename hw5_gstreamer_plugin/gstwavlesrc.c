/* GStreamer
 * Copyright (C) 2023 FIXME <fixme@example.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */
/**
 * SECTION:element-gstwavlesrc
 *
 * The wavlesrc element does FIXME stuff.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v fakesrc ! wavlesrc ! FIXME ! fakesink
 * ]|
 * FIXME Describe what the pipeline does.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/gst.h>
#include <gst/base/gstbasesrc.h>
#include "gstwavlesrc.h"

GST_DEBUG_CATEGORY_STATIC (gst_wavlesrc_debug_category);
#define GST_CAT_DEFAULT gst_wavlesrc_debug_category

/* prototypes */


static void gst_wavlesrc_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_wavlesrc_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_wavlesrc_dispose (GObject * object);
static void gst_wavlesrc_finalize (GObject * object);

static GstCaps *gst_wavlesrc_get_caps (GstBaseSrc * src, GstCaps * filter);
static gboolean gst_wavlesrc_negotiate (GstBaseSrc * src);
static GstCaps *gst_wavlesrc_fixate (GstBaseSrc * src, GstCaps * caps);
static gboolean gst_wavlesrc_set_caps (GstBaseSrc * src, GstCaps * caps);
static gboolean gst_wavlesrc_decide_allocation (GstBaseSrc * src,
    GstQuery * query);
static gboolean gst_wavlesrc_start (GstBaseSrc * src);
static gboolean gst_wavlesrc_stop (GstBaseSrc * src);
static void gst_wavlesrc_get_times (GstBaseSrc * src, GstBuffer * buffer,
    GstClockTime * start, GstClockTime * end);
static gboolean gst_wavlesrc_get_size (GstBaseSrc * src, guint64 * size);
static gboolean gst_wavlesrc_is_seekable (GstBaseSrc * src);
static gboolean gst_wavlesrc_prepare_seek_segment (GstBaseSrc * src,
    GstEvent * seek, GstSegment * segment);
static gboolean gst_wavlesrc_do_seek (GstBaseSrc * src, GstSegment * segment);
static gboolean gst_wavlesrc_unlock (GstBaseSrc * src);
static gboolean gst_wavlesrc_unlock_stop (GstBaseSrc * src);
static gboolean gst_wavlesrc_query (GstBaseSrc * src, GstQuery * query);
static gboolean gst_wavlesrc_event (GstBaseSrc * src, GstEvent * event);
static GstFlowReturn gst_wavlesrc_create (GstBaseSrc * src, guint64 offset,
    guint size, GstBuffer ** buf);
static GstFlowReturn gst_wavlesrc_alloc (GstBaseSrc * src, guint64 offset,
    guint size, GstBuffer ** buf);
static GstFlowReturn gst_wavlesrc_fill (GstBaseSrc * src, guint64 offset,
    guint size, GstBuffer * buf);

enum
{
  PROP_0,
  PROP_LOCATION
};

/* pad templates */

static GstStaticPadTemplate gst_wavlesrc_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw")
    );


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstWavLESrc, gst_wavlesrc, GST_TYPE_BASE_SRC,
  GST_DEBUG_CATEGORY_INIT (gst_wavlesrc_debug_category, "wavlesrc", 0,
  "debug category for wavlesrc element"));

static void
gst_wavlesrc_class_init (GstWavLESrcClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseSrcClass *base_src_class = GST_BASE_SRC_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS(klass),
      &gst_wavlesrc_src_template);

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS(klass),
      "WAV audio source", "Generic", "plays audio from wav files",
      "<example@example.com>");

  gobject_class->set_property = gst_wavlesrc_set_property;
  gobject_class->get_property = gst_wavlesrc_get_property;
  gobject_class->dispose = gst_wavlesrc_dispose;
  gobject_class->finalize = gst_wavlesrc_finalize;
  base_src_class->get_caps = GST_DEBUG_FUNCPTR (gst_wavlesrc_get_caps);
  base_src_class->negotiate = GST_DEBUG_FUNCPTR (gst_wavlesrc_negotiate);
  base_src_class->fixate = GST_DEBUG_FUNCPTR (gst_wavlesrc_fixate);
  base_src_class->set_caps = GST_DEBUG_FUNCPTR (gst_wavlesrc_set_caps);
  base_src_class->decide_allocation = GST_DEBUG_FUNCPTR (gst_wavlesrc_decide_allocation);
  base_src_class->start = GST_DEBUG_FUNCPTR (gst_wavlesrc_start);
  base_src_class->stop = GST_DEBUG_FUNCPTR (gst_wavlesrc_stop);
  base_src_class->get_times = GST_DEBUG_FUNCPTR (gst_wavlesrc_get_times);
  base_src_class->get_size = GST_DEBUG_FUNCPTR (gst_wavlesrc_get_size);
  base_src_class->is_seekable = GST_DEBUG_FUNCPTR (gst_wavlesrc_is_seekable);
  base_src_class->prepare_seek_segment = GST_DEBUG_FUNCPTR (gst_wavlesrc_prepare_seek_segment);
  base_src_class->do_seek = GST_DEBUG_FUNCPTR (gst_wavlesrc_do_seek);
  base_src_class->unlock = GST_DEBUG_FUNCPTR (gst_wavlesrc_unlock);
  base_src_class->unlock_stop = GST_DEBUG_FUNCPTR (gst_wavlesrc_unlock_stop);
  base_src_class->query = GST_DEBUG_FUNCPTR (gst_wavlesrc_query);
  base_src_class->event = GST_DEBUG_FUNCPTR (gst_wavlesrc_event);
  base_src_class->create = GST_DEBUG_FUNCPTR (gst_wavlesrc_create);
  base_src_class->alloc = GST_DEBUG_FUNCPTR (gst_wavlesrc_alloc);
  base_src_class->fill = GST_DEBUG_FUNCPTR (gst_wavlesrc_fill);

  g_object_class_install_property(
    gobject_class, PROP_LOCATION,
    g_param_spec_string("location", "location", "WAV file location", "",
                        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS)
  );
}

static void
gst_wavlesrc_init (GstWavLESrc *wavlesrc)
{
}

void
gst_wavlesrc_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (object);

  GST_DEBUG_OBJECT (wavlesrc, "set_property");

  switch (property_id) {
    case PROP_LOCATION:
      wavlesrc->location = g_strdup(g_value_get_string(value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_wavlesrc_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (object);

  GST_DEBUG_OBJECT (wavlesrc, "get_property");

  switch (property_id) {
    case PROP_LOCATION:
      g_value_set_string(value, wavlesrc->location);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_wavlesrc_dispose (GObject * object)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (object);

  GST_DEBUG_OBJECT (wavlesrc, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS (gst_wavlesrc_parent_class)->dispose (object);
}

void
gst_wavlesrc_finalize (GObject * object)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (object);

  GST_DEBUG_OBJECT (wavlesrc, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS (gst_wavlesrc_parent_class)->finalize (object);
}

/* get caps from subclass */
static GstCaps *
gst_wavlesrc_get_caps (GstBaseSrc * src, GstCaps * filter)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "get_caps");

  return NULL;
}

/* decide on caps */
static gboolean
gst_wavlesrc_negotiate (GstBaseSrc * src)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "negotiate");

  return TRUE;
}

/* called if, in negotiation, caps need fixating */
static GstCaps *
gst_wavlesrc_fixate (GstBaseSrc * src, GstCaps * caps)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "fixate");

  return NULL;
}

/* notify the subclass of new caps */
static gboolean
gst_wavlesrc_set_caps (GstBaseSrc * src, GstCaps * caps)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "set_caps");

  return TRUE;
}

/* setup allocation query */
static gboolean
gst_wavlesrc_decide_allocation (GstBaseSrc * src, GstQuery * query)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "decide_allocation");

  return TRUE;
}

/* start and stop processing, ideal for opening/closing the resource */
static gboolean
gst_wavlesrc_start (GstBaseSrc * src)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "start");

  return TRUE;
}

static gboolean
gst_wavlesrc_stop (GstBaseSrc * src)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "stop");

  return TRUE;
}

/* given a buffer, return start and stop time when it should be pushed
 * out. The base class will sync on the clock using these times. */
static void
gst_wavlesrc_get_times (GstBaseSrc * src, GstBuffer * buffer,
    GstClockTime * start, GstClockTime * end)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "get_times");

}

/* get the total size of the resource in bytes */
static gboolean
gst_wavlesrc_get_size (GstBaseSrc * src, guint64 * size)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "get_size");

  return TRUE;
}

/* check if the resource is seekable */
static gboolean
gst_wavlesrc_is_seekable (GstBaseSrc * src)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "is_seekable");

  return TRUE;
}

/* Prepare the segment on which to perform do_seek(), converting to the
 * current basesrc format. */
static gboolean
gst_wavlesrc_prepare_seek_segment (GstBaseSrc * src, GstEvent * seek,
    GstSegment * segment)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "prepare_seek_segment");

  return TRUE;
}

/* notify subclasses of a seek */
static gboolean
gst_wavlesrc_do_seek (GstBaseSrc * src, GstSegment * segment)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "do_seek");

  return TRUE;
}

/* unlock any pending access to the resource. subclasses should unlock
 * any function ASAP. */
static gboolean
gst_wavlesrc_unlock (GstBaseSrc * src)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "unlock");

  return TRUE;
}

/* Clear any pending unlock request, as we succeeded in unlocking */
static gboolean
gst_wavlesrc_unlock_stop (GstBaseSrc * src)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "unlock_stop");

  return TRUE;
}

/* notify subclasses of a query */
static gboolean
gst_wavlesrc_query (GstBaseSrc * src, GstQuery * query)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "query");

  return GST_BASE_SRC_CLASS(gst_wavlesrc_parent_class)->query(src, query);
}

/* notify subclasses of an event */
static gboolean
gst_wavlesrc_event (GstBaseSrc * src, GstEvent * event)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "event");

  return TRUE;
}

/* ask the subclass to create a buffer with offset and size, the default
 * implementation will call alloc and fill. */
static GstFlowReturn
gst_wavlesrc_create (GstBaseSrc * src, guint64 offset, guint size,
    GstBuffer ** buf)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "create");

  return GST_FLOW_OK;
}

/* ask the subclass to allocate an output buffer. The default implementation
 * will use the negotiated allocator. */
static GstFlowReturn
gst_wavlesrc_alloc (GstBaseSrc * src, guint64 offset, guint size,
    GstBuffer ** buf)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "alloc");

  return GST_FLOW_OK;
}

/* ask the subclass to fill the buffer with data from offset and size */
static GstFlowReturn
gst_wavlesrc_fill (GstBaseSrc * src, guint64 offset, guint size, GstBuffer * buf)
{
  GstWavLESrc *wavlesrc = GST_WAVLESRC (src);

  GST_DEBUG_OBJECT (wavlesrc, "fill");

  return GST_FLOW_OK;
}

static gboolean
plugin_init (GstPlugin * plugin)
{

  /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
  return gst_element_register (plugin, "wavlesrc", GST_RANK_NONE,
      GST_TYPE_WAVLESRC);
}

/* FIXME: these are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION "0.0.FIXME"
#endif
#ifndef PACKAGE
#define PACKAGE "FIXME_package"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "FIXME_package_name"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "http://FIXME.org/"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    wavlesrc,
    "FIXME plugin description",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)

