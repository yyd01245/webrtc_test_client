/*
 *  Copyright 2012 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/examples/webrtc_client/src/linux/main_wnd.h"

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <stddef.h>

#include "libyuv/convert_from.h"
#include "webrtc/api/video/i420_buffer.h"
#include "webrtc/examples/webrtc_client/src/defaults.h"
#include "webrtc/base/common.h"
#include "webrtc/base/logging.h"
#include "webrtc/base/stringutils.h"

using rtc::sprintfn;

namespace {

//
// Simple static functions that simply forward the callback to the
// GtkMainWnd instance.
//

gboolean OnDestroyedCallback(GtkWidget* widget, GdkEvent* event,
                             gpointer data) {
  reinterpret_cast<GtkMainWnd*>(data)->OnDestroyed(widget, event);
  return FALSE;
}

void OnClickedCallback(GtkWidget* widget, gpointer data) {
  reinterpret_cast<GtkMainWnd*>(data)->OnClicked(widget);
}

gboolean SimulateButtonClick(gpointer button) {
  g_signal_emit_by_name(button, "clicked");
  return false;
}

gboolean OnKeyPressCallback(GtkWidget* widget, GdkEventKey* key,
                            gpointer data) {
  reinterpret_cast<GtkMainWnd*>(data)->OnKeyPress(widget, key);
  return false;
}

void OnRowActivatedCallback(GtkTreeView* tree_view, GtkTreePath* path,
                            GtkTreeViewColumn* column, gpointer data) {
  LOG(INFO) << __FUNCTION__<<" Callback activated line " << __LINE__;
  reinterpret_cast<GtkMainWnd*>(data)->OnRowActivated(tree_view, path, column);
  // reinterpret_cast<GtkMainWnd*>(data)->ConnectCallback(1);
}

gboolean SimulateLastRowActivated(gpointer data) {
    LOG(INFO) << __FUNCTION__<<" row activated line " << __LINE__;
  GtkTreeView* tree_view = reinterpret_cast<GtkTreeView*>(data);
  GtkTreeModel* model = gtk_tree_view_get_model(tree_view);

  // "if iter is NULL, then the number of toplevel nodes is returned."
  int rows = gtk_tree_model_iter_n_children(model, NULL);
  GtkTreePath* lastpath = gtk_tree_path_new_from_indices(rows - 1, -1);
    LOG(INFO) << __FUNCTION__<<" row activated line " << __LINE__;
  // Select the last item in the list
  GtkTreeSelection* selection = gtk_tree_view_get_selection(tree_view);
  gtk_tree_selection_select_path(selection, lastpath);

  // Our TreeView only has one column, so it is column 0.
  GtkTreeViewColumn* column = gtk_tree_view_get_column(tree_view, 0);
    LOG(INFO) << __FUNCTION__<<" row activated line " << __LINE__;
  gtk_tree_view_row_activated(tree_view, lastpath, column);
    LOG(INFO) << __FUNCTION__<<" row activated line " << __LINE__;
  gtk_tree_path_free(lastpath);
  return false;
}

// Creates a tree view, that we use to display the list of peers.
void InitializeList(GtkWidget* list) {
    LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes(
      "List Items", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
  GtkListStore* store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
  gtk_tree_view_set_model(GTK_TREE_VIEW(list), GTK_TREE_MODEL(store));
  g_object_unref(store);
}

// Adds an entry to a tree view.
void AddToList(GtkWidget* list, const gchar* str,uint64_t  value) {
  GtkListStore* store = GTK_LIST_STORE(
      gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
      LOG(INFO)<<__FUNCTION__<<" ID "<<value;
  GtkTreeIter iter;
  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, 0, str, 1, value, -1);
}

struct UIThreadCallbackData {
  explicit UIThreadCallbackData(MainWndCallback* cb, int id, void* d)
      : callback(cb), msg_id(id), data(d) {}
  MainWndCallback* callback;
  int msg_id;
  void* data;
};

// gboolean HandleUIThreadCallback(gpointer data) {
//   UIThreadCallbackData* cb_data = reinterpret_cast<UIThreadCallbackData*>(data);
//   //fixme
//   // cb_data->callback->UIThreadCallback(cb_data->msg_id, cb_data->data);
//   LOG(INFO)<<__FUNCTION__<<" HANDLEUI CALLBACK "<< cb_data->msg_id;
//   delete cb_data;
//   return false;
// }

gboolean Redraw(gpointer data) {
  GtkMainWnd* wnd = reinterpret_cast<GtkMainWnd*>(data);
  wnd->OnRedraw();
  return false;
}

gboolean Draw(GtkWidget* widget, cairo_t* cr, gpointer data) {
  GtkMainWnd* wnd = reinterpret_cast<GtkMainWnd*>(data);
  wnd->Draw(widget, cr);
  return false;
}

}  // namespace

//
// GtkMainWnd implementation.
//

GtkMainWnd::GtkMainWnd(const char* server, int port, bool autoconnect,
                       bool autocall)
    : window_(NULL), draw_area_(NULL), vbox_(NULL), server_edit_(NULL),
      port_edit_(NULL), peer_list_(NULL), callback_(NULL),
      server_(server), autoconnect_(autoconnect), autocall_(autocall) {
  char buffer[10];
    LOG(INFO) << __FUNCTION__<<" line " << __LINE__ 
        <<" autoconnect "<<autoconnect_<<" server "
        << server<<" port " <<port;
  sprintfn(buffer, sizeof(buffer), "%i", port);
  port_ = buffer;
}

GtkMainWnd::~GtkMainWnd() {
  ASSERT(!IsWindow());
}

void GtkMainWnd::RegisterObserver(MainWndCallback* callback) {
  callback_ = callback;
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
}

bool GtkMainWnd::IsWindow() {
  return window_ != NULL && GTK_IS_WINDOW(window_);
}

void GtkMainWnd::MessageBox(const char* caption, const char* text,
                            bool is_error) {
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(window_),
      GTK_DIALOG_DESTROY_WITH_PARENT,
      is_error ? GTK_MESSAGE_ERROR : GTK_MESSAGE_INFO,
      GTK_BUTTONS_CLOSE, "%s", text);
  gtk_window_set_title(GTK_WINDOW(dialog), caption);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

MainWindow::UI GtkMainWnd::current_ui() {
  if (vbox_)
    return CONNECT_TO_SERVER;

  if (peer_list_)
    return LIST_PEERS;

  return STREAMING;
}


void GtkMainWnd::StartLocalRenderer(webrtc::VideoTrackInterface* local_video) {
  local_renderer_.reset(new VideoRenderer(this, local_video));
}

void GtkMainWnd::StopLocalRenderer() {
  local_renderer_.reset();
}

void GtkMainWnd::StartRemoteRenderer(
    webrtc::VideoTrackInterface* remote_video) {
  remote_renderer_.reset(new VideoRenderer(this, remote_video));
}

void GtkMainWnd::StopRemoteRenderer() {
  remote_renderer_.reset();
}

void GtkMainWnd::QueueUIThreadCallback(int msg_id, void* data) {
  //fixme
  // g_idle_add(HandleUIThreadCallback,
  //            new UIThreadCallbackData(callback_, msg_id, data));
}

bool GtkMainWnd::Create() {
  ASSERT(window_ == NULL);
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  window_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  if (window_) {
    gtk_window_set_position(GTK_WINDOW(window_), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window_), 640, 480);
    gtk_window_set_title(GTK_WINDOW(window_), "PeerConnection client");
    g_signal_connect(G_OBJECT(window_), "delete-event",
                     G_CALLBACK(&OnDestroyedCallback), this);
    g_signal_connect(window_, "key-press-event", G_CALLBACK(OnKeyPressCallback),
                     this);

    SwitchToConnectUI();
  }
  printf("new window success \n");

  return window_ != NULL;
}

bool GtkMainWnd::Destroy() {
  if (!IsWindow())
    return false;

  gtk_widget_destroy(window_);
  window_ = NULL;

  return true;
}

void GtkMainWnd::SwitchToConnectUI() {
  LOG(INFO) << __FUNCTION__;

  ASSERT(IsWindow());
  ASSERT(vbox_ == NULL);

  gtk_container_set_border_width(GTK_CONTAINER(window_), 10);

  if (peer_list_) {
    gtk_widget_destroy(peer_list_);
    peer_list_ = NULL;
  }
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
#if GTK_MAJOR_VERSION == 2
  vbox_ = gtk_vbox_new(FALSE, 5);
#else
  vbox_ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
#endif
  GtkWidget* valign = gtk_alignment_new(0, 1, 0, 0);
  gtk_container_add(GTK_CONTAINER(vbox_), valign);
  gtk_container_add(GTK_CONTAINER(window_), vbox_);
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
#if GTK_MAJOR_VERSION == 2
  GtkWidget* hbox = gtk_hbox_new(FALSE, 5);
#else
  GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
#endif
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  GtkWidget* label = gtk_label_new("Server");
  gtk_container_add(GTK_CONTAINER(hbox), label);

  server_edit_ = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(server_edit_), server_.c_str());
  gtk_widget_set_size_request(server_edit_, 400, 30);
  gtk_container_add(GTK_CONTAINER(hbox), server_edit_);

  port_edit_ = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(port_edit_), port_.c_str());
  gtk_widget_set_size_request(port_edit_, 70, 30);
  gtk_container_add(GTK_CONTAINER(hbox), port_edit_);

  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;

  GtkWidget* button = gtk_button_new_with_label("Connect");
  gtk_widget_set_size_request(button, 70, 30);
  g_signal_connect(button, "clicked", G_CALLBACK(OnClickedCallback), this);
  gtk_container_add(GTK_CONTAINER(hbox), button);

  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;

  GtkWidget* halign = gtk_alignment_new(1, 0, 0, 0);
  gtk_container_add(GTK_CONTAINER(halign), hbox);
  gtk_box_pack_start(GTK_BOX(vbox_), halign, FALSE, FALSE, 0);

  gtk_widget_show_all(window_);

  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;

  if (autoconnect_)
    g_idle_add(SimulateButtonClick, button);
}

void GtkMainWnd::SwitchToPeerList(const Peers& peers) {
  LOG(INFO) << __FUNCTION__;

  if (!peer_list_) {
    gtk_container_set_border_width(GTK_CONTAINER(window_), 0);
    if (vbox_) {
      gtk_widget_destroy(vbox_);
      vbox_ = NULL;
      server_edit_ = NULL;
      port_edit_ = NULL;
    } else if (draw_area_) {
      gtk_widget_destroy(draw_area_);
      draw_area_ = NULL;
      draw_buffer_.reset();
    }
  LOG(INFO) << __FUNCTION__<<"SwitchToPeerList line " << __LINE__;
    peer_list_ = gtk_tree_view_new();
    g_signal_connect(peer_list_, "row-activated",
                     G_CALLBACK(OnRowActivatedCallback), this);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(peer_list_), FALSE);
    InitializeList(peer_list_);
    gtk_container_add(GTK_CONTAINER(window_), peer_list_);
    gtk_widget_show_all(window_);
  } else {
    GtkListStore* store =
        GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(peer_list_)));
    gtk_list_store_clear(store);
  }
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  AddToList(peer_list_, "List of currently connected peers:", 0);
  for (Peers::const_iterator i = peers.begin(); i != peers.end(); ++i)
    AddToList(peer_list_, i->second.c_str(), i->first);

// autocall_
  if (autocall_ && peers.begin() != peers.end())
    g_idle_add(SimulateLastRowActivated, peer_list_);

}

void GtkMainWnd::SwitchToStreamingUI() {
  LOG(INFO) << __FUNCTION__;

  ASSERT(draw_area_ == NULL);

  gtk_container_set_border_width(GTK_CONTAINER(window_), 0);
  if (peer_list_) {
    gtk_widget_destroy(peer_list_);
    peer_list_ = NULL;
  }
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  draw_area_ = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(window_), draw_area_);
  g_signal_connect(G_OBJECT(draw_area_), "draw", G_CALLBACK(&::Draw), this);
  LOG(INFO) << __FUNCTION__<<" line " << __LINE__;
  gtk_widget_show_all(window_);
}

void GtkMainWnd::OnDestroyed(GtkWidget* widget, GdkEvent* event) {
  callback_->Close();
  window_ = NULL;
  draw_area_ = NULL;
  vbox_ = NULL;
  server_edit_ = NULL;
  port_edit_ = NULL;
  peer_list_ = NULL;
}

void GtkMainWnd::OnClicked(GtkWidget* widget) {
  // Make the connect button insensitive, so that it cannot be clicked more than
  // once.  Now that the connection includes auto-retry, it should not be
  // necessary to click it more than once.
  gtk_widget_set_sensitive(widget, false);
  server_ = gtk_entry_get_text(GTK_ENTRY(server_edit_));
  port_ = gtk_entry_get_text(GTK_ENTRY(port_edit_));
  int port = port_.length() ? atoi(port_.c_str()) : 0;
  callback_->StartLogin(server_, port);
}

void GtkMainWnd::OnKeyPress(GtkWidget* widget, GdkEventKey* key) {
  if (key->type == GDK_KEY_PRESS) {
    switch (key->keyval) {
#if GTK_MAJOR_VERSION == 2
      case GDK_Escape:
#else
      case GDK_KEY_Escape:
#endif
        if (draw_area_) {
          callback_->DisconnectFromCurrentPeer();
        } else if (peer_list_) {
          callback_->DisconnectFromServer();
        }
        break;

#if GTK_MAJOR_VERSION == 2
      case GDK_KP_Enter:
      case GDK_Return:
#else
      case GDK_KEY_KP_Enter:
      case GDK_KEY_Return:
#endif
        if (vbox_) {
          OnClicked(NULL);
        } else if (peer_list_) {
          // OnRowActivated will be called automatically when the user
          // presses enter.
        }
        break;

      default:
        break;
    }
  }
}

gboolean EmptyCallback(gpointer data) {
  static bool connect = true;
  if(connect == false){
    return false;
  }
  connect = false;
    LOG(INFO) << __FUNCTION__ << "onpeerconnect peerid";

  GtkMainWnd* this0 = reinterpret_cast<GtkMainWnd*>(data);
  this0->Peercallback();
  return true;
}
void GtkMainWnd::Peercallback(){
    LOG(INFO) << __FUNCTION__ << "onpeerconnect peerid 1";
  uint64_t id = 1;
  callback_->ConnectToPeer(id);
      LOG(INFO) << __FUNCTION__ << "onpeerconnect peerid 2";
}

void GtkMainWnd::ConnectCallback(uint64_t id){
  LOG(INFO) << __FUNCTION__<<" callback line " << __LINE__<< " id= "<<id;
  // callback_->ConnectToPeer(id);
  // SimulateLastRowActivated(peer_list_);

  //  g_idle_add(SimulateLastRowActivated, peer_list_);
   g_idle_add(EmptyCallback, this);
  // g_signal_emit_by_name(peer_list_,"row-activated");
}

void GtkMainWnd::OnRowActivated(GtkTreeView* tree_view, GtkTreePath* path,
                                GtkTreeViewColumn* column) {
  LOG(INFO) << __FUNCTION__<<" OnRowActivated line " << __LINE__;

  ASSERT(peer_list_ != NULL);
  GtkTreeIter iter;
  GtkTreeModel* model;
  GtkTreeSelection* selection =
      gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
  if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
     char* text;
     uint64_t id = 0;
     gtk_tree_model_get(model, &iter, 0, &text, 1, &id,  -1);
      LOG(INFO)<< "connectToPeer GET LIST ID = "<<id;
     if (id != 0)
       callback_->ConnectToPeer(id);
     g_free(text);
  }
  // callback_->ConnectToPeer(1);

}

void GtkMainWnd::OnRedraw() {
  gdk_threads_enter();

  VideoRenderer* remote_renderer = remote_renderer_.get();
  if (remote_renderer && remote_renderer->image() != NULL &&
      draw_area_ != NULL) {
    width_ = remote_renderer->width();
    height_ = remote_renderer->height();

    if (!draw_buffer_.get()) {
      draw_buffer_size_ = (width_ * height_ * 4) * 4;
      draw_buffer_.reset(new uint8_t[draw_buffer_size_]);
      gtk_widget_set_size_request(draw_area_, width_ * 2, height_ * 2);
    }

    const uint32_t* image =
        reinterpret_cast<const uint32_t*>(remote_renderer->image());
    uint32_t* scaled = reinterpret_cast<uint32_t*>(draw_buffer_.get());
    for (int r = 0; r < height_; ++r) {
      for (int c = 0; c < width_; ++c) {
        int x = c * 2;
        scaled[x] = scaled[x + 1] = image[c];
      }

      uint32_t* prev_line = scaled;
      scaled += width_ * 2;
      memcpy(scaled, prev_line, (width_ * 2) * 4);

      image += width_;
      scaled += width_ * 2;
    }

    VideoRenderer* local_renderer = local_renderer_.get();
    if (local_renderer && local_renderer->image()) {
      image = reinterpret_cast<const uint32_t*>(local_renderer->image());
      scaled = reinterpret_cast<uint32_t*>(draw_buffer_.get());
      // Position the local preview on the right side.
      scaled += (width_ * 2) - (local_renderer->width() / 2);
      // right margin...
      scaled -= 10;
      // ... towards the bottom.
      scaled += (height_ * width_ * 4) - ((local_renderer->height() / 2) *
                                          (local_renderer->width() / 2) * 4);
      // bottom margin...
      scaled -= (width_ * 2) * 5;
      for (int r = 0; r < local_renderer->height(); r += 2) {
        for (int c = 0; c < local_renderer->width(); c += 2) {
          scaled[c / 2] = image[c + r * local_renderer->width()];
        }
        scaled += width_ * 2;
      }
    }

#if GTK_MAJOR_VERSION == 2
    gdk_draw_rgb_32_image(draw_area_->window,
                          draw_area_->style->fg_gc[GTK_STATE_NORMAL], 0, 0,
                          width_ * 2, height_ * 2, GDK_RGB_DITHER_MAX,
                          draw_buffer_.get(), (width_ * 2) * 4);
#else
    gtk_widget_queue_draw(draw_area_);
#endif
  }

  gdk_threads_leave();
}

void GtkMainWnd::Draw(GtkWidget* widget, cairo_t* cr) {
#if GTK_MAJOR_VERSION != 2
  cairo_format_t format = CAIRO_FORMAT_RGB24;
  cairo_surface_t* surface = cairo_image_surface_create_for_data(
      draw_buffer_.get(), format, width_ * 2, height_ * 2,
      cairo_format_stride_for_width(format, width_ * 2));
  cairo_set_source_surface(cr, surface, 0, 0);
  cairo_rectangle(cr, 0, 0, width_ * 2, height_ * 2);
  cairo_fill(cr);
  cairo_surface_destroy(surface);
#else
  RTC_NOTREACHED();
#endif
}

GtkMainWnd::VideoRenderer::VideoRenderer(
    GtkMainWnd* main_wnd,
    webrtc::VideoTrackInterface* track_to_render)
    : width_(0),
      height_(0),
      main_wnd_(main_wnd),
      rendered_track_(track_to_render) {
  rendered_track_->AddOrUpdateSink(this, rtc::VideoSinkWants());
}

GtkMainWnd::VideoRenderer::~VideoRenderer() {
  rendered_track_->RemoveSink(this);
}

void GtkMainWnd::VideoRenderer::SetSize(int width, int height) {
  gdk_threads_enter();

  if (width_ == width && height_ == height) {
    return;
  }

  width_ = width;
  height_ = height;
  image_.reset(new uint8_t[width * height * 4]);
  gdk_threads_leave();
}

void GtkMainWnd::VideoRenderer::OnFrame(
    const webrtc::VideoFrame& video_frame) {
  gdk_threads_enter();

  rtc::scoped_refptr<webrtc::VideoFrameBuffer> buffer(
      video_frame.video_frame_buffer());
  if (video_frame.rotation() != webrtc::kVideoRotation_0) {
    buffer = webrtc::I420Buffer::Rotate(*buffer, video_frame.rotation());
  }
  SetSize(buffer->width(), buffer->height());

  libyuv::I420ToRGBA(buffer->DataY(), buffer->StrideY(),
                     buffer->DataU(), buffer->StrideU(),
                     buffer->DataV(), buffer->StrideV(),
                     image_.get(), width_ * 4,
                     buffer->width(), buffer->height());

  gdk_threads_leave();

  g_idle_add(Redraw, main_wnd_);
}
