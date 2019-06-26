/*
 * Copyright (c) 2018 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <plugins/gbp/gbp_ext_itf.h>
#include <plugins/gbp/gbp_bridge_domain.h>
#include <plugins/gbp/gbp_route_domain.h>
#include <plugins/gbp/gbp_itf.h>

/**
 * Pool of GBP ext_itfs
 */
gbp_ext_itf_t *gbp_ext_itf_pool;

/**
 * external interface configs keyed by sw_if_index
 */
index_t *gbp_ext_itf_db;

#define GBP_EXT_ITF_ID 0x00000080

/**
 * logger
 */
vlib_log_class_t gx_logger;

#define GBP_EXT_ITF_DBG(...)                           \
    vlib_log_debug (gx_logger, __VA_ARGS__);

u8 *
format_gbp_ext_itf (u8 * s, va_list * args)
{
  gbp_ext_itf_t *gx = va_arg (*args, gbp_ext_itf_t *);

  return (format (s, "%U in %U",
		  format_gbp_itf, gx->gx_itf,
		  format_gbp_bridge_domain, gx->gx_bd));
}

int
gbp_ext_itf_add (u32 sw_if_index, u32 bd_id, u32 rd_id)
{
  gbp_ext_itf_t *gx;
  index_t gxi;

  vec_validate_init_empty (gbp_ext_itf_db, sw_if_index, INDEX_INVALID);

  gxi = gbp_ext_itf_db[sw_if_index];

  if (INDEX_INVALID == gxi)
    {
      gbp_route_domain_t *gr;
      fib_protocol_t fproto;
      index_t gbi, gri;

      gbi = gbp_bridge_domain_find_and_lock (bd_id);

      if (INDEX_INVALID == gbi)
	return (VNET_API_ERROR_NO_SUCH_ENTRY);

      gri = gbp_route_domain_find_and_lock (rd_id);

      if (INDEX_INVALID == gri)
	{
	  gbp_bridge_domain_unlock (gbi);
	  return (VNET_API_ERROR_NO_SUCH_ENTRY);
	}

      pool_get_zero (gbp_ext_itf_pool, gx);
      gxi = gx - gbp_ext_itf_pool;

      gr = gbp_route_domain_get (gri);

      gx->gx_bd = gbi;
      gx->gx_rd = gri;
      gx->gx_itf = sw_if_index;

      FOR_EACH_FIB_IP_PROTOCOL (fproto)
      {
	gx->gx_fib_index[fproto] =
	  gr->grd_fib_index[fib_proto_to_dpo (fproto)];
      }

      gbp_ext_itf_db[sw_if_index] = gxi;

      GBP_EXT_ITF_DBG ("add: %U", format_gbp_ext_itf, gx);

      return (0);
    }

  return (VNET_API_ERROR_ENTRY_ALREADY_EXISTS);
}

int
gbp_ext_itf_delete (u32 sw_if_index)
{
  gbp_ext_itf_t *gx;
  index_t gxi;

  if (vec_len (gbp_ext_itf_db) <= sw_if_index)
    return (VNET_API_ERROR_INVALID_SW_IF_INDEX);

  gxi = gbp_ext_itf_db[sw_if_index];

  if (INDEX_INVALID != gxi)
    {
      gx = pool_elt_at_index (gbp_ext_itf_pool, gxi);

      GBP_EXT_ITF_DBG ("del: %U", format_gbp_ext_itf, gx);

      gbp_route_domain_unlock (gx->gx_rd);
      gbp_bridge_domain_unlock (gx->gx_bd);

      gbp_ext_itf_db[sw_if_index] = INDEX_INVALID;
      pool_put (gbp_ext_itf_pool, gx);

      return (0);
    }
  return (VNET_API_ERROR_NO_SUCH_ENTRY);
}

int
gbp_ext_itf_anon_add (u32 sw_if_index, u32 bd_id, u32 rd_id)
{
  int rv = gbp_ext_itf_add (sw_if_index, bd_id, rd_id);
  if (rv)
    return rv;
  /* add interface to the BD */
  index_t itf = gbp_itf_add_and_lock (sw_if_index, bd_id);
  /* setup GBP L2 features on this interface */
  gbp_itf_set_l2_input_feature (itf, 0,
				L2INPUT_FEAT_GBP_LPM_ANON_CLASSIFY |
				L2INPUT_FEAT_LEARN);
  gbp_itf_set_l2_output_feature (itf, 0, L2OUTPUT_FEAT_GBP_POLICY_LPM);
  return 0;
}

int
gbp_ext_itf_anon_delete (u32 sw_if_index)
{
  int rv = gbp_ext_itf_delete (sw_if_index);
  if (rv)
    return rv;
  gbp_itf_unlock (sw_if_index);
  return 0;
}

static clib_error_t *
gbp_ext_itf_add_del_cli (vlib_main_t * vm,
			 unformat_input_t * input, vlib_cli_command_t * cmd)
{
  unformat_input_t _line_input, *line_input = &_line_input;
  u32 sw_if_index = ~0, bd_id = ~0, rd_id = ~0;
  int add = 1, anon = 0;
  int rv;

  /* Get a line of input. */
  if (!unformat_user (input, unformat_line_input, line_input))
    return 0;

  while (unformat_check_input (line_input) != UNFORMAT_END_OF_INPUT)
    {
      if (unformat (line_input, "del"))
	add = 0;
      else
	if (unformat
	    (line_input, "%U", unformat_vnet_sw_interface, vnet_get_main (),
	     &sw_if_index))
	;
      else if (unformat (line_input, "bd %d", &bd_id))
	;
      else if (unformat (line_input, "rd %d", &rd_id))
	;
      else if (unformat (line_input, "anon-l3-out"))
	anon = 1;
      else
	return clib_error_return (0, "unknown input `%U'",
				  format_unformat_error, line_input);
    }
  unformat_free (line_input);

  if (~0 == sw_if_index)
    return clib_error_return (0, "interface must be specified");

  if (add)
    {
      if (~0 == bd_id)
	return clib_error_return (0, "BD-ID must be specified");
      if (~0 == rd_id)
	return clib_error_return (0, "RD-ID must be specified");
      if (anon)
	rv = gbp_ext_itf_anon_add (sw_if_index, bd_id, rd_id);
      else
	rv = gbp_ext_itf_add (sw_if_index, bd_id, rd_id);
    }
  else
    {
      if (anon)
	rv = gbp_ext_itf_anon_delete (sw_if_index);
      else
	rv = gbp_ext_itf_delete (sw_if_index);
    }

  switch (rv)
    {
    case 0:
      return 0;
    case VNET_API_ERROR_ENTRY_ALREADY_EXISTS:
      return clib_error_return (0, "interface already exists");
    case VNET_API_ERROR_NO_SUCH_ENTRY:	/* fallthrough */
    case VNET_API_ERROR_INVALID_SW_IF_INDEX:
      return clib_error_return (0, "unknown interface");
    default:
      return clib_error_return (0, "error %d", rv);
    }

  /* never reached */
  return 0;
}

/*?
 * Add Group Based Interface as anonymous L3out interface
 *
 * @cliexpar
 * @cliexstart{gbp interface [del] anon-l3out <interface> bd <ID>}
 * @cliexend
 ?*/
/* *INDENT-OFF* */
VLIB_CLI_COMMAND (gbp_itf_anon_l3out_add_del_node, static) = {
  .path = "gbp ext-itf",
  .short_help = "gbp ext-itf [del] <interface> bd <ID> rd <ID> [anon-l3-out]\n",
  .function = gbp_ext_itf_add_del_cli,
};
/* *INDENT-ON* */

void
gbp_ext_itf_walk (gbp_ext_itf_cb_t cb, void *ctx)
{
  gbp_ext_itf_t *ge;

  /* *INDENT-OFF* */
  pool_foreach(ge, gbp_ext_itf_pool,
  {
    if (!cb(ge, ctx))
      break;
  });
  /* *INDENT-ON* */
}

static walk_rc_t
gbp_ext_itf_show_one (gbp_ext_itf_t * gx, void *ctx)
{
  vlib_cli_output (ctx, "  %U", format_gbp_ext_itf, gx);

  return (WALK_CONTINUE);
}

static clib_error_t *
gbp_ext_itf_show (vlib_main_t * vm,
		  unformat_input_t * input, vlib_cli_command_t * cmd)
{
  vlib_cli_output (vm, "External-Interfaces:");
  gbp_ext_itf_walk (gbp_ext_itf_show_one, vm);

  return (NULL);
}

/*?
 * Show Group Based Policy external interface and derived information
 *
 * @cliexpar
 * @cliexstart{show gbp ext-itf}
 * @cliexend
 ?*/
/* *INDENT-OFF* */
VLIB_CLI_COMMAND (gbp_ext_itf_show_node, static) = {
  .path = "show gbp ext-itf",
  .short_help = "show gbp ext-itf\n",
  .function = gbp_ext_itf_show,
};
/* *INDENT-ON* */

static clib_error_t *
gbp_ext_itf_init (vlib_main_t * vm)
{
  gx_logger = vlib_log_register_class ("gbp", "ext-itf");

  return (NULL);
}

VLIB_INIT_FUNCTION (gbp_ext_itf_init);

/*
 * fd.io coding-style-patch-verification: ON
 *
 * Local Variables:
 * eval: (c-set-style "gnu")
 * End:
 */
