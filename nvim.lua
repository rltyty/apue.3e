local map = vim.keymap.set
map('n', '<leader>mk', '<Cmd>make BUILD=Release -s<CR>', {noremap = true})
map('n', '<leader>md', '<Cmd>make BUILD=Debug -s<CR>', {noremap = true})
map('n', '<leader>mt', '<Cmd>make BUILD=Debug -s test<CR>', {noremap = true})
map('n', '<leader>mc', '<Cmd>make -s clean<CR>', {noremap = true})

-- Refer to $HOME/.local/share/nvim/lazy/mason-nvim-dap.nvim/lua/mason-nvim-dap/mappings/configurations.lua:82
local prj_dap_c = {
  {
    name = "C Debug Launch: current file (project + args)",
    type = "codelldb",
    request = "launch",
    program = "${workspaceFolder}/Debug/${relativeFileDirname}/${fileBasenameNoExtension}",
    args = function()
      local input = vim.fn.input('Args: ')
      return vim.split(input, ' ')  -- Split into a table
    end,
  },
  {
    name = "C Debug Launch: current file (standalone + args)",
    type = "codelldb",
    request = "launch",
    program = "${workspaceFolder}/${relativeFileDirname}/${fileBasenameNoExtension}",
    args = function()
      local input = vim.fn.input('Args: ')
      return vim.split(input, ' ')  -- Split into a table
    end,
  }
}

local dap_c = require('dap').configurations.c
dap_c = vim.list_extend(dap_c or {}, prj_dap_c)

