if vim.g.vimrclevel <= 1 then return end

local map = vim.keymap.set

map('n', '<leader>mk', '<Cmd>make -j$(nproc) BUILD=Release -s<CR>', { noremap = true })
map('n', '<leader>md', '<Cmd>make -j$(nproc) BUILD=Debug <CR>', { noremap = true })
map('n', '<leader>mt', '<Cmd>make -j$(nproc) BUILD=Debug -s test<CR>', { noremap = true })
map('n', '<leader>mc', '<Cmd>make -j$(nproc) -s clean<CR>', { noremap = true })

-- test makefile
map('n', '<leader>mdt', '<Cmd>make -j$(nproc) BUILD=Debug SHARED=Y VERSION=3<CR>', { noremap = true })

-- Refer to $HOME/.local/share/nvim/lazy/mason-nvim-dap.nvim/lua/mason-nvim-dap/mappings/configurations.lua:82
local prj_dap_c = {
  {
    name = "C Debug Launch: current file (project + args)",
    type = "codelldb",
    request = "launch",
    program = "${workspaceFolder}/Debug/${relativeFileDirname}/${fileBasenameNoExtension}",
    args = function()
      local input = vim.fn.input('Args: ')
      return vim.split(input, ' ') -- Split into a table
    end,
  },
  {
    name = "C Debug Launch: current file (standalone + args)",
    type = "codelldb",
    request = "launch",
    program = "${workspaceFolder}/${relativeFileDirname}/${fileBasenameNoExtension}",
    args = function()
      local input = vim.fn.input('Args: ')
      return vim.split(input, ' ') -- Split into a table
    end,
  }
}

local dap_c = require('dap').configurations.c
dap_c = vim.list_extend(dap_c or {}, prj_dap_c)


-- project specific templates

vim.g.use_prj_tmpl = true
vim.g.prj_tmpl_root = 'templates'

-- add separator
map({ 'n', 'v' }, '<leader>zs', '70a-<ESC>', { noremap = true, desc = 'Insert a sep line.' })
