local map = vim.keymap.set
map('n', '<leader>mk', '<Cmd>make BUILD=Release -s<CR>', {noremap = true})
map('n', '<leader>md', '<Cmd>make BUILD=Debug -s<CR>', {noremap = true})
map('n', '<leader>mt', '<Cmd>make BUILD=Debug -s test<CR>', {noremap = true})
map('n', '<leader>mc', '<Cmd>make -s clean<CR>', {noremap = true})
