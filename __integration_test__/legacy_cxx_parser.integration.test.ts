import fs from "fs";
import os from "os";
import path from "path";

import { TerraContext } from "@agoraio-extensions/terra-core";
import { LegacyCXXParser } from "../src/legacy_cxx_parser";
import { CXXFile, Struct } from "@agoraio-extensions/cxx-parser";

describe("legacy_cxx_parser", () => {
  let tmpDir: string = "";
  let cppastBackendPath: string = "";

  beforeEach(() => {
    tmpDir = fs.mkdtempSync(path.join(os.tmpdir(), "terra-ut-"));
  });

  afterEach(() => {
    fs.rmSync(tmpDir, { recursive: true, force: true });
  });

  describe("LegacyCXXParser", () => {
    it("can parse the header smoke test", () => {
      let file1Name = "file1.h";
      let file1Path = path.join(tmpDir, file1Name);

      fs.writeFileSync(
        file1Path,
        `
#pragma once

namespace ns1 {
  struct AAA {
      int aaa_;

      AAA(): aaa_(0) {}
      AAA(int aaa): aaa_(aaa) {}
  };
}
`
      );

      let args = {
        language: "c++",
        includeHeaderDirs: [],
        definesMacros: [],
        parseFiles: { include: [file1Name] },
        customHeaders: [],
      };

      LegacyCXXParser(new TerraContext(tmpDir, tmpDir), args, undefined)!;
    });
  });
});
