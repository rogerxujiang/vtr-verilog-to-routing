#ifndef RR_GRAPH2_H
#define RR_GRAPH2_H
#include <vector>

#include "build_switchblocks.h"
#include "rr_graph_util.h"
#include "rr_types.h"
#include "device_grid.h"

/******************* Types shared by rr_graph2 functions *********************/

enum e_seg_details_type {
	SEG_DETAILS_X, SEG_DETAILS_Y
};

struct t_rr_edge_info {
    t_rr_edge_info(int from, int to, int type)
        : from_node(from), to_node(to), switch_type(type) {}

    int from_node = OPEN;
    int to_node = OPEN;
    int switch_type = OPEN;

    friend bool operator<(const t_rr_edge_info& lhs, const t_rr_edge_info& rhs) {
        return std::tie(lhs.from_node, lhs.to_node, lhs.switch_type) <
               std::tie(rhs.from_node, rhs.to_node, rhs.switch_type);
    }
};

typedef std::set<t_rr_edge_info> t_rr_edge_info_set;

/******************* Subroutines exported by rr_graph2.c *********************/

t_rr_node_indices alloc_and_load_rr_node_indices(
		const int max_chan_width,
        const DeviceGrid& grid,
		int *index,
		const t_chan_details& chan_details_x,
		const t_chan_details& chan_details_y);

int get_rr_node_index(
		int x, int y,
		t_rr_type rr_type, int ptc,
		const t_rr_node_indices& L_rr_node_indices);

//Returns all the rr nodes associated with the specified coordinate (i.e. accross sides)
std::vector<int> get_rr_node_indices(const t_rr_node_indices& L_rr_node_indices,
                                     int x, int y, t_rr_type rr_type, int ptc);

//Return the first rr node of the specified type and coordinates
// For non-IPIN/OPIN types 'side' is ignored
int get_rr_node_index(const t_rr_node_indices& L_rr_node_indices,
        int x, int y, t_rr_type rr_type, int ptc, e_side side=NUM_SIDES);

int find_average_rr_node_index(
		int device_width, int device_height,
		t_rr_type rr_type, int ptc,
		const t_rr_node_indices&L_rr_node_indices);

t_seg_details *alloc_and_load_seg_details(
		int *max_chan_width,
		const int max_len,
		const int num_seg_types,
		const t_segment_inf *segment_inf,
		const bool use_full_seg_groups,
		const bool is_global_graph,
		const enum e_directionality directionality,
		int *num_seg_details = nullptr);

void alloc_and_load_chan_details(
        const DeviceGrid& grid,
		const t_chan_width *nodes_per_chan,
		const bool trim_empty_channels,
		const bool trim_obs_channels,
		const int num_seg_details,
		const t_seg_details *seg_details,
		t_chan_details& chan_details_x,
		t_chan_details& chan_details_y);
t_chan_details init_chan_details(
        const DeviceGrid& grid,
		const t_chan_width *nodes_per_chan,
		const int num_seg_details,
		const t_seg_details *seg_details,
		const enum e_seg_details_type seg_details_type);
void obstruct_chan_details(
        const DeviceGrid& grid,
		const t_chan_width *nodes_per_chan,
		const bool trim_empty_channels,
		const bool trim_obs_channels,
		t_chan_details& chan_details_x,
		t_chan_details& chan_details_y);
void adjust_chan_details(
        const DeviceGrid& grid,
		const t_chan_width *nodes_per_chan,
		t_chan_details& chan_details_x,
		t_chan_details& chan_details_y);
void adjust_seg_details(
		const int x, const int y,
        const DeviceGrid& grid,
		const t_chan_width *nodes_per_chan,
		t_chan_details& chan_details,
		const enum e_seg_details_type seg_details_type);

void free_seg_details(
		t_seg_details *seg_details,
		const int max_chan_width);
void free_chan_details(
		t_chan_details& chan_details_x,
		t_chan_details& chan_details_y,
		const int max_chan_width,
        const DeviceGrid& grid);

int get_seg_start(
		const t_seg_details *seg_details,
		const int itrack,
		const int chan_num,
		const int seg_num);
int get_seg_end(const t_seg_details *seg_details,
		const int itrack,
		const int istart,
		const int chan_num,
		const int seg_max);

bool is_cblock(
		const int chan,
		const int seg,
		const int track,
		const t_seg_details *seg_details);

bool is_sblock(
		const int chan,
		int wire_seg,
		const int sb_seg,
		const int track,
		const t_seg_details *seg_details,
		const enum e_directionality directionality);

int get_bidir_opin_connections(
		const int i, const int j,
		const int ipin,
        const int from_rr_node,
        t_rr_edge_info_set& rr_edges_to_create,
		const t_pin_to_track_lookup& opin_to_track_map,
		const t_rr_node_indices& L_rr_node_indices,
		const t_seg_details *seg_details);

int get_unidir_opin_connections(
		const int chan,
		const int seg,
		int Fc,
		const int seg_type_index,
		const t_rr_type chan_type,
		const t_seg_details *seg_details,
        const int from_rr_node,
        t_rr_edge_info_set& rr_edges_to_create,
        vtr::NdMatrix<int,3>& Fc_ofs,
		const int max_len,
		const int max_chan_width,
		const t_rr_node_indices& L_rr_node_indices,
		bool *Fc_clipped);

int get_track_to_pins(
		int seg, int chan, int track, int tracks_per_chan,
        int from_rr_node,
        t_rr_edge_info_set& rr_edges_to_create,
        const t_rr_node_indices& L_rr_node_indices,
		const t_track_to_pin_lookup& track_to_pin_lookup, t_seg_details *seg_details,
		enum e_rr_type chan_type, int chan_length, int wire_to_ipin_switch,
		enum e_directionality directionality);

int get_track_to_tracks(
		const int from_chan,
		const int from_seg,
		const int from_track,
		const t_rr_type from_type,
		const int to_seg,
		const t_rr_type to_type,
		const int chan_len,
		const int max_chan_width,
        const DeviceGrid& grid,
		const int Fs_per_side,
		short ******sblock_pattern,
        const int from_rr_node,
        t_rr_edge_info_set& rr_edges_to_create,
		const t_seg_details *from_seg_details,
		const t_seg_details *to_seg_details,
		const t_chan_details& to_chan_details,
		const enum e_directionality directionality,
		const t_rr_node_indices& L_rr_node_indices,
		const vtr::NdMatrix<std::vector<int>,3>& switch_block_conn,
		t_sb_connection_map *sb_conn_map);

short ******alloc_sblock_pattern_lookup(
        const DeviceGrid& grid,
		const int max_chan_width);
void free_sblock_pattern_lookup(
		short ******sblock_pattern);
void load_sblock_pattern_lookup(
        const int i, const int j,
        const DeviceGrid& grid,
		const t_chan_width *nodes_per_chan,
		const t_chan_details& chan_details_x,
		const t_chan_details& chan_details_y,
		const int Fs,
		const enum e_switch_block_type switch_block_type,
		short ******sblock_pattern);

int *get_seg_track_counts(
		const int num_sets, const int num_seg_types,
		const t_segment_inf * segment_inf, const bool use_full_seg_groups);

void dump_seg_details(
		const t_seg_details *seg_details,
		int max_chan_width,
		const char *fname);
void dump_seg_details(
		const t_seg_details *seg_details,
		int max_chan_width,
		FILE *fp);
void dump_chan_details(
		const t_chan_details& chan_details_x,
		const t_chan_details& chan_details_y,
		int max_chan_width,
        const DeviceGrid& grid,
		const char *fname);
void dump_sblock_pattern(
		short ******sblock_pattern,
		int max_chan_width,
		const DeviceGrid& grid,
		const char *fname);

//Partitions RR graph edges to allow fast access to configurable/non-configurabe edge subsets
void partition_rr_graph_edges(DeviceContext& device_ctx);

#endif
