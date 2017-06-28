{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "index.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
