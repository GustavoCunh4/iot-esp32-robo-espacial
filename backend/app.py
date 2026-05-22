import os
from typing import Any

import requests
from flask import Flask, jsonify, request
from dotenv import load_dotenv


app = Flask(__name__)

load_dotenv()


SUPABASE_URL = os.getenv("SUPABASE_URL", "https://SEU-PROJETO.supabase.co/rest/v1/leituras")
SUPABASE_KEY = os.getenv("SUPABASE_KEY", "SUA_SUPABASE_KEY")


def supabase_headers() -> dict[str, str]:
  return {
      "apikey": SUPABASE_KEY,
      "Authorization": f"Bearer {SUPABASE_KEY}",
      "Content-Type": "application/json",
  }


@app.get("/health")
def health() -> Any:
  return jsonify({"status": "ok"})


@app.get("/leituras")
def listar_leituras() -> Any:
  limite = request.args.get("limit", default=20, type=int)
  limite = max(1, min(limite, 100))

  try:
    resposta = requests.get(
        SUPABASE_URL,
        headers=supabase_headers(),
        params={"select": "*", "order": "id.desc", "limit": limite},
        timeout=15,
    )
    resposta.raise_for_status()
  except requests.RequestException as exc:
    return jsonify({"erro": "falha_ao_consultar_supabase", "detalhe": str(exc)}), 502

  return (
      jsonify(
          {
              "origem": "supabase",
              "quantidade": len(resposta.json()),
              "dados": resposta.json(),
          }
      ),
      resposta.status_code,
  )


if __name__ == "__main__":
  app.run(host="0.0.0.0", port=8000, debug=True)
