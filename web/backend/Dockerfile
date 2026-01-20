FROM python:3.10-slim
WORKDIR /app

# Allow overriding the trivial_db binary path at build/run time
ARG TRIVIAL_BIN=/app/build/trivial_db
ENV TRIVIAL_BIN=${TRIVIAL_BIN}

# Copy application sources
COPY . /app

# If the trivial_db binary is present in the build context, copy it into the image.
# The repository already contains `build/trivial_db` so this will include the binary.
COPY build/trivial_db /app/build/trivial_db
RUN chmod +x /app/build/trivial_db || true

# Install build dependencies required for some Python packages (bcrypt)
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    python3-dev \
    libffi-dev \
    libssl-dev \
 && pip install --no-cache-dir -r /app/web/backend/requirements.txt \
 && pip install --no-cache-dir bcrypt \
 && apt-get remove -y --purge build-essential \
 && apt-get autoremove -y \
 && rm -rf /var/lib/apt/lists/*
EXPOSE 8000
CMD ["uvicorn", "app:app", "--host", "0.0.0.0", "--port", "8000"]


